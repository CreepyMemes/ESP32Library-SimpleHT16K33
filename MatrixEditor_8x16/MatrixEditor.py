import tkinter as tk

class LEDMatrixEditor:
    def __init__(self, master):
        self.master = master
        self.master.title("LED Matrix Editor")
        self.matrix = [[0] * 16 for _ in range(8)]

        # Create the grid of buttons
        self.buttons = [[tk.Button(master, width=3, height=1, command=lambda row=row, col=col: self.toggle_pixel(row, col)) for col in range(16)] for row in range(8)]

        for row in range(8):
            for col in range(16):
                self.buttons[row][col].grid(row=row, column=col)

        # Generate button
        generate_button = tk.Button(master, text="Generate Bitmap", command=self.generate_bitmap)
        generate_button.grid(row=8, column=0, columnspan=16)

    def toggle_pixel(self, row, col):
        self.matrix[row][col] = 1 - self.matrix[row][col]  # Toggle pixel state
        self.update_button_color(row, col)

    def update_button_color(self, row, col):
        color = "white" if self.matrix[row][col] == 0 else "black"
        self.buttons[row][col].configure(bg=color)

    def generate_bitmap(self):
        bitmap_array = []
        for row in range(8):
            row_value = 0
            for col in range(16):
                row_value |= (self.matrix[row][col] << (15 - col))
            bitmap_array.append(row_value)

        # Print the generated bitmap array
        print("static const uint16_t")
        print("    bitmap[] = {")
        for value in bitmap_array:
            print(f"        0b{format(value, '016b')},")
        print("    };")

if __name__ == "__main__":
    root = tk.Tk()
    editor = LEDMatrixEditor(root)
    root.mainloop()
