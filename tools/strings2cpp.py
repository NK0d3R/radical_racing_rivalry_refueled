import math
import argparse
import struct
import re
import os

MAX_STRLEN = 31
SIZE_BITS = int(math.log(MAX_STRLEN + 1, 2))
OFFSET_BITS = 16 - SIZE_BITS

def writeHeader(filepath, guardname, copyright, data, tab_str):
    with open(filepath, "wt") as output:
        if copyright != None:
            output.write(copyright)
        output.write("#ifndef %s_H_\n#define %s_H_\n\n" %
                     (guardname, guardname))
        output.write("enum Strings : uint8_t {\n");
        for idx, item in enumerate(data):
            if idx == 0:
                output.write("%s%s = 0,\n" % (tab_str, item[0]))
            else:
                output.write("%s%s,\n" % (tab_str, item[0]))
        output.write(tab_str + "StringsNb\n};\n\n")
        output.write("#endif  // %s_H_\n" % (guardname))

def writeDataFile(filepath, guardname, copyright, data, maxitems, tab_str):
    with open(filepath, "wt") as output:
        if copyright != None:
            output.write(copyright)
        output.write("#ifndef %s_HPP_\n#define %s_HPP_\n\n" %
                     (guardname, guardname))
        output.write("PROGMEM const uint16_t STRINGINFO[] = {\n")
        crtoffset = 0
        offsetmask = 2 ** OFFSET_BITS - 1
        sizemask = 2 ** SIZE_BITS - 1
        for idx, item in enumerate(data):
            crtsize = len(item[1])
            if crtoffset > offsetmask or crtsize > sizemask:
                raise Exception("Strings size or index" +
                                " cannot be represented: %d %d" %
                                (crtoffset, crtsize))
            crtvalue = crtoffset | (crtsize << OFFSET_BITS)
            output.write("%s%s" % (tab_str, format(crtvalue, "#06x")))
            if idx < len(data) - 1:
                output.write(",\n")
            crtoffset += crtsize
        output.write("\n};\n\n")
        output.write("PROGMEM const char STRINGDATA[] = {");
        alldata = ""
        for item in data:
            alldata += item[1]
        crtlineitem = 0
        for idx, value in enumerate(alldata):
            if crtlineitem == 0:
                output.write("\n%s" % (tab_str))
            output.write(format(ord(value), "#04x"))
            if idx < len(alldata) - 1:
                output.write(",")
            crtlineitem += 1
            if crtlineitem == maxitems or idx == len(alldata) - 1:
                crtlineitem = 0
            else:
                output.write(" ")
        output.write("\n};\n\n")
        output.write("#endif  // %s_HPP_\n" % (guardname))

def main():
    parser = argparse.ArgumentParser(description="String file to CPP converter")
    parser.add_argument("-i", "--input",
                        help="Input file to process", required=True)
    parser.add_argument("-o", "--output",
                        help="Output file name", required=True)
    parser.add_argument("-m", "--maxvals", help="Max number of values per line",
                        type=int, default=16, required=False)
    parser.add_argument("-t", "--tabs", help="Tab size in spaces",
                        type=int, default=4, required=False)
    parser.add_argument("-c", "--copyright", help="Copyright file",
                        default="", required=False)
    options = parser.parse_args()
    matcher = re.compile(r"([^0-9][a-zA-Z0-9_-]*)\s+?[\"\'](.*?)[\"\']")
    stringdata = []
    copyrightdata = None
    tab_str = " " * options.tabs

    if options.copyright != "":
        with open(options.copyright, "rt") as copyright:
            copyrightdata = copyright.read()

    filename = os.path.basename(options.output)
    dirname = os.path.dirname(options.output)
    basename = os.path.splitext(filename)[0]
    guardname = basename.upper().replace(" ", "_").replace(".","_")

    with open(options.input, "r") as input:
        linedata = input.readlines()
        input.close()

        for idx, line in enumerate(linedata):
            line = line.strip()
            if line.startswith("#"):
                continue
            result = matcher.match(line)
            if not result:
                print("Warning: Invalid string at line %d" % (idx + 1))
                continue
            data = result.groups()
            if len(data[1]) == 0:
                print("Warning: Empty string at line %d" % (idx + 1))
                continue
            if len(data[1]) > MAX_STRLEN:
                raise Exception("String: %s too long (%d)" %
                                (data[0], len(data[1])))
            stringdata.append(data)

    writeHeader(os.path.join(dirname, basename + ".h"), guardname,
                copyrightdata, stringdata, tab_str)
    writeDataFile(os.path.join(dirname, basename + ".hpp"), guardname,
                  copyrightdata, stringdata, options.maxvals, tab_str)

if __name__ == "__main__":
    main()