from PIL import Image
import numpy as np
#import matplotlib.pyplot as plt

def image_to_bitmap_array(image_path):
    
    # Fixed width
    target_width = 384

    # Open the image
    img = Image.open(image_path)

    # Convert to grayscale
    img = img.convert("L")

    # Calculate the new height to maintain aspect ratio
    original_width, original_height = img.size
    target_height = round((target_width / original_width) * original_height)

    # Resize the image
    img = img.resize((target_width, target_height), Image.Resampling.LANCZOS)

    # Convert to binary image (thresholding)
    threshold = 128
    lookup_table = [0 if i < threshold else 1 for i in range(256)]
    img = img.point(lookup_table, '1')

    # Convert to numpy array
    bitmap_array = np.array(img, dtype=np.uint8)

    return img, bitmap_array

def convert_to_c_hex(bitmap_array):
    c_code = []
    for row in bitmap_array:
        hex_row = []
        for i in range(0, len(row), 8):
            byte = 0
            for bit in row[i:i+8]:
                byte = (byte << 1) | bit
            hex_row.append(f"0x{byte:02x}")
        c_code.append( ", ".join(hex_row) )
    return c_code

# Example usage
if __name__ == "__main__":
    image_path = "/Users/zeongit/Desktop/WechatIMG83.jpg"  # Replace with your image path
    processed_img, bitmap = image_to_bitmap_array(image_path)

    # Show the processed binary image
    processed_img.show()
    c_hex_rows = convert_to_c_hex(bitmap)

    # Print the bitmap array
    """for row in bitmap:
        print(row)"""

    for row in c_hex_rows:
        print(row)
