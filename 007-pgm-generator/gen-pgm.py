import os

def type_to_int(d):
    if type(d) == str:
        return 0
    if type(d) == float:
        return 1
    if type(d) == int:
        return 2
    return -1

def main():
    height = 256
    width = 256
    slices = 14
    channels = 3
    frames = 5

    metadata = {
        'height': height,
        'width': width,
        'slices': slices,
        'channels': channels,
        'frames': frames,
        'pixel size x': 0.035,
        'pixel size y': 0.035,
        'pixel size z': 0.1,
        'unit x': "um",
        'unit y': "µm",
        'unit z': "um",
        'interval': 5,
        'time unit': "s",
        'model': "Zeiss AxioImager 4",
        'lut c1': "random",
        'lut c2': "grays",
        'lut c3': "cyan",
        'format': "AscII"
    }

    export_path = "/home/clement/Desktop/test-img.pgm"

    data_size = height * width * slices * channels * frames
    step = 1.0 / data_size

    data = [100.0 * i * step for i in range(data_size)]

    with open(export_path, 'w') as f:
        for key, item in metadata.items():
            slot = f"{key};{type_to_int(item)};{item}\n"
            f.write(slot)
        ascii_data = ";".join([str(round(i, 3)) for i in data]) + "\n"
        f.write(ascii_data)


if __name__ == "__main__":
    main()