import cv2
import numpy as np
from smbus2 import SMBus

# Global variables
cap = None
webcam_index = 0
width = 640
height = 480
auto_exposure = True
exposure_value = 200  # Default exposure value, adjustable via this variable
top_line_y = 220  # Y-coordinate of the top line
bottom_line_y = 225  # Y-coordinate of the bottom line
i2c_address = 13  # I2C address of the slave
bus = SMBus(1)  # I2C bus number (1 for Raspberry Pi)

def highlight_colors(frame):
    # Convert the frame to HSV color space
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # Define color ranges for red, green, and magenta with multiple shades
    red_ranges = [
        ([0, 70, 90], [10, 255, 255]),
        ([170, 70, 50], [180, 255, 255])
    ]
    green_ranges = [
        ([40, 70, 100], [90, 255, 255]),
        ([40, 120, 70], [90, 255, 255])
    ]
    magenta_ranges = [
        ([140, 50, 50], [170, 255, 255])
    ]

    # Function to create a mask for a list of ranges
    def create_mask(hsv_image, ranges):
        mask = np.zeros(hsv_image.shape[:2], dtype="uint8")
        for (lower, upper) in ranges:
            lower = np.array(lower)
            upper = np.array(upper)
            mask = cv2.bitwise_or(mask, cv2.inRange(hsv_image, lower, upper))
        return mask

    # Create masks for each color
    mask_red = create_mask(hsv, red_ranges)
    mask_green = create_mask(hsv, green_ranges)
    mask_magenta = create_mask(hsv, magenta_ranges)

    # Create an output frame where colors are highlighted
    highlighted_frame = frame.copy()

    # Draw the top and bottom lines
    cv2.line(highlighted_frame, (0, top_line_y), (width, top_line_y), (255, 255, 255), 2)
    cv2.line(highlighted_frame, (0, bottom_line_y), (width, bottom_line_y), (255, 255, 255), 2)

    # Find contours for each color
    colors = [
        (mask_red, 'R', (0, 0, 255)),
        (mask_green, 'G', (0, 255, 0)),
        (mask_magenta, 'M', (255, 0, 255))
    ]
    
    contours_info = []
    for color_mask, color_letter, color_value in colors:
        contours, _ = cv2.findContours(color_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            area = cv2.contourArea(contour)
            if area >= 400:  # Ensuring at least 25x25 pixels
                x, y, w, h = cv2.boundingRect(contour)
                if y + h > top_line_y and y < bottom_line_y:  # Ensure the contour is between the lines
                    contours_info.append((contour, color_letter, color_value, area, x, y, w, h))

    # Sort contours by area in descending order
    contours_info = sorted(contours_info, key=lambda x: x[3], reverse=True)

    # Filter the contours to follow the specific rules
    selected_contours = []
    red_count = 0
    green_count = 0
    magenta_count = 0
    for contour, color_letter, color_value, area, x, y, w, h in contours_info:
        if len(selected_contours) < 3:
            if color_letter == 'R' and red_count < 2:
                selected_contours.append((contour, color_letter, color_value, x, y, w, h))
                red_count += 1
            elif color_letter == 'G' and green_count < 2:
                selected_contours.append((contour, color_letter, color_value, x, y, w, h))
                green_count += 1
            elif color_letter == 'M' and magenta_count < 1:
                selected_contours.append((contour, color_letter, color_value, x, y, w, h))
                magenta_count += 1

    # Ensure at least one magenta object if there are three objects
    if len(selected_contours) == 3 and magenta_count == 0:
        for i in range(len(selected_contours)):
            if selected_contours[i][1] in ['R', 'G']:
                del selected_contours[i]
                break
        for contour, color_letter, color_value, area, x, y, w, h in contours_info:
            if color_letter == 'M' and color_letter not in [sc[1] for sc in selected_contours]:
                selected_contours.append((contour, color_letter, color_value, x, y, w, h))
                break

    # Create a two-dimensional array for tracked objects
    tracked_objects = [[-1, -1, -1, -1, -1] for _ in range(3)]
    for i, (contour, color_letter, color_value, x, y, w, h) in enumerate(selected_contours):
        color_value = int(ord(color_letter))
        tracked_objects[i] = [color_value, int(x), int(y), int(w), int(h)]

    # Send data to I2C slave
    send_data_i2c(tracked_objects)

    return highlighted_frame

def send_data_i2c(data):
    flattened_data = [item for sublist in data for item in sublist]
    byte_data = []
    for value in flattened_data:
        byte_data.append((value >> 8) & 0xFF)  # High byte
        byte_data.append(value & 0xFF)         # Low byte
    try:
        bus.write_i2c_block_data(i2c_address, 0, byte_data)
        print(f"Data sent to I2C: {byte_data}")
    except Exception as e:
        print(f"Failed to send data via I2C: {e}")

def initialize_camera():
    global cap, webcam_index, width, height, auto_exposure, exposure_value
    if cap is not None:
        cap.release()
    cap = cv2.VideoCapture(webcam_index, cv2.CAP_V4L2)
    if cap.isOpened():
        cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
        if auto_exposure:
            cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 1)
        else:
            cap.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0)
            cap.set(cv2.CAP_PROP_EXPOSURE, exposure_value)
        print(f"Webcam {webcam_index} initialized.")
    else:
        print(f"Error: Could not open webcam with index {webcam_index}.")
        cap = None

def main():
    global cap, auto_exposure, exposure_value

    # Initialize the camera with the given parameters
    initialize_camera()

    while True:
        if cap is not None and cap.isOpened():
            ret, frame = cap.read()
            if ret:
                highlighted_frame = highlight_colors(frame)
                # Process the highlighted_frame as needed
                # You can add code here to save the frame, analyze it further, etc.
                # For demonstration, we'll just print a message
                print("Frame processed.")
                # Implement a break condition as needed
                if break_condition():  # Replace with your break condition
                    break

    # Release the webcam
    if cap is not None:
        cap.release()

def break_condition():
    # Implement your break condition here
    # For example, you can break after a certain number of frames
    # or based on some external input
    return False

if __name__ == "__main__":
    main()
