import argparse
import sys
import os

def main():
    parser = argparse.ArgumentParser(
                        description="Binary to C declaration converter")
    parser.add_argument("-i", "--input", help="Input file to process",
                        required=True)
    parser.add_argument("-o", "--output", help="Output C file",
                        required=True)
    parser.add_argument("-m", "--maxvals", help="Max number of values per line",
                        type=int, default=16, required=False)
    parser.add_argument("-t", "--tabs", help="Tab size in spaces",
                        type=int, default=4, required=False)
    parser.add_argument("-c", "--copyright", help="Copyright file",
                        default="", required=False)
    options = parser.parse_args()

    buffer_size = 1024 * 512
    file_size = 0

    try:
        file_size = os.path.getsize(options.input)
    except OSError:
        print("Could not stat file")
        sys.exit(-1)

    filename = os.path.basename(options.output)
    basename = os.path.splitext(filename)[0]

    variable_name_base = basename.upper().replace(" ", "_").replace(".","_")
    variable_name_size = variable_name_base + "_SIZE"
    variable_name_data = variable_name_base + "_DATA"
    tab_str = " " * options.tabs

    with open(options.input, "rb") as input:
        with open(options.output, "wt") as output:
            if options.copyright != "":
                with open(options.copyright, "rt") as copyright:
                    output.write(copyright.read())
            output.write("#ifndef %s_H_\n#define %s_H_\n\n" %
                         (variable_name_base, variable_name_base))
            output.write("#define %s%s%d\n" % (
                         variable_name_size, tab_str, file_size))
            output.write("PROGMEM const uint8_t %s[] = {" %
                         (variable_name_data))

            crt_line_byte = 0
            needs_newline = True
            needs_final_newline = True
            while True:
                chunk = input.read(buffer_size)
                if chunk:
                    for idx, byte in enumerate(chunk):
                        if needs_newline:
                            output.write("\n%s" % (tab_str))
                            needs_newline = False
                            needs_final_newline = False
                        output.write("%s," %
                                     format(ord(byte), "#04x"))
                        crt_line_byte += 1
                        if crt_line_byte == options.maxvals:
                            needs_newline = True
                            crt_line_byte = 0
                        else:
                            if idx < len(chunk) - 1:
                                output.write(" ")
                            else:
                                needs_final_newline = True
                else:
                    break
                if needs_newline or needs_final_newline:
                    output.write("\n")
                output.write("};\n\n#endif  // %s_H_\n" %
                             (variable_name_base))

if __name__ == "__main__":
    main()
