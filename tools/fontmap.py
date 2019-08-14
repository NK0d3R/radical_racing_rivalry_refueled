import argparse
import sys
import string
import os

def main():
    parser = argparse.ArgumentParser(
                description="Generate source file for a font mapping")
    parser.add_argument("-i", "--input", action="store",
                        required=True,
                        help="File containing font mapping definition")
    parser.add_argument("-o", "--output", action="store",
                        required=True, help="Output file")
    parser.add_argument("-t", "--tabs", help="Tab size in spaces",
                        type=int, default=4, required=False)
    parser.add_argument("-c", "--copyright", help="Copyright file",
                        default="", required=False)
    parse_data = parser.parse_args()
    data_lines = None
    with open(parse_data.input, 'r') as input:
        data_lines = input.readlines()
    if data_lines is None or len(data_lines) == 0:
        print("Could not read input file !")
        sys.exit(255)
    default_mapping = -1
    max_char = -1
    min_char = 256
    tab_str = " " * parse_data.tabs
    mapping = {}

    for index, line in enumerate(data_lines):
        elements = line.split()
        if len(elements) < 2:
            print("Skipping line %d" % (index + 1))
            continue
        if elements[0] == "default":
            default_mapping = int(elements[-1])
        else:
            used_frame = int(elements[-1])
            for char in elements[:-1]:
                if len(char) == 1:
                    chr_value = ord(char)
                    if chr_value > max_char:
                        max_char = chr_value
                    if chr_value < min_char:
                        min_char = chr_value
                    mapping[chr_value] = used_frame
                else:
                    print("Invalid char sequence at line %d" % (index + 1))

    filename = os.path.basename(parse_data.output)
    basename = os.path.splitext(filename)[0]
    variable_name_base = basename.upper().replace(" ", "_").replace(".","_")

    with open(parse_data.output, 'w') as output:
        if parse_data.copyright != "":
            with open(parse_data.copyright, "rt") as copyright:
                output.write(copyright.read())
        output.write("#ifndef %s_H_\n#define %s_H_\n\n" %
                     (variable_name_base, variable_name_base))
        output.write("uint8_t nb_map_elems = %d;\n" % (max_char - min_char + 1))
        output.write("uint8_t map_start_char = %d;\n" % (min_char))
        output.write("uint8_t default_frame = %d;\n" % (default_mapping))
        output.write("PROGMEM uint8_t const mapping[] = {\n")
        for char in range(min_char, max_char + 1):
            stop = False
            outstr = ""
            if char in mapping:
                outstr = "%s%d" % (tab_str, mapping[char])
                if char == max_char:
                    stop = True
                else:
                    outstr += ","
            else:
                outstr = "%s%d," % (tab_str, default_mapping)
            crt_char = chr(char)
            if crt_char.isspace() is False and crt_char in string.printable:
                outstr += "%s/* '%c\' */" % (tab_str, crt_char)
            outstr += "\n"
            output.write(outstr)
            if stop:
                break
        output.write("};\n\n#endif  // %s_H_\n" % (variable_name_base))

if __name__=="__main__":
    main()