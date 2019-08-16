import argparse
import os

parser = argparse.ArgumentParser(
                description="HTML color list file to C declaration converter")

def main():
    parser.add_argument("-i", "--input", help="Input file to process",
                        required=True)
    parser.add_argument("-o", "--output", help="Output H file",
                        required=True)
    parser.add_argument("-c", "--copyright", help="Copyright file",
                        default="", required=False)
    parser.add_argument("-t", "--tabs", help="Tab size in spaces",
                        type=int, default=4, required=False)
    options = parser.parse_args()

    filename = os.path.basename(options.output)
    basename = os.path.splitext(filename)[0]

    variable_name_base = basename.upper().replace(" ", "_").replace(".","_")
    variable_name_size = variable_name_base + "_SIZE"
    tab_str = " " * options.tabs

    with open(options.input, "rb") as input:
        with open(options.output, "wt") as output:
            data = input.readlines()
            if len(data) == 0:
                raise Exception("No data in file!")
            colors = []
            for color in data:
                color = color.strip()
                if len(color) == 0:
                    continue
                if len(color) != 6:
                    raise Exception("Invalid format, RRGGBB required")
                red = int(color[0:2], 16)
                green = int(color[2:4], 16)
                blue = int(color[4:6], 16)
                red = (red >> 3)
                green = (green >> 2)
                blue = (blue >> 3)
                resultColor = (red | (green << 5) | (blue << 11))
                colors.append(
                    ((resultColor & 0xFF00) >> 8) |
                    ((resultColor & 0xFF) << 8))
            if options.copyright != "":
                with open(options.copyright, "rt") as copyright:
                    output.write(copyright.read())
            output.write("#ifndef %s_H_\n#define %s_H_\n\n" %
                            (variable_name_base, variable_name_base))
            output.write("#define %s%s%d\n" % (
                            variable_name_size, tab_str, len(colors)))
            output.write("PROGMEM const uint16_t %s[] = {\n" %
                            (variable_name_base))
            for color in colors:
                output.write("%s0x%04x,\n" % (tab_str, color))
            output.write("};\n\n#endif  // %s_H_\n" % (variable_name_base))

if __name__=="__main__":
    main()