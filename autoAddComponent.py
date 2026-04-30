import os
import re

comp_structs = []

def read_file(filename):
    with open(filename, "r", encoding="utf-8", errors="ignore") as f:
        dest = [l[:-1] for l in f.readlines()]
    return (dest)

files_list = []

temp_list = os.listdir()

while (1):
    cur_len = len(temp_list)
    for dir in temp_list:
        if (os.path.isdir(dir)):
            temp_list += [f"{dir}/{file}" for file in os.listdir(dir)]
            temp_list.remove(dir)
    if (len(temp_list) == cur_len):
        break

good_formats = [".hpp", ".cpp"]

for file in temp_list:
    if (not os.path.isfile(file)):
        continue

    for form in good_formats:
        if (file.endswith(form)):
            files_list.append(file)
            continue

def find_file(name):
    global files_list
    for f in files_list:
        if (f.endswith(name)):
            return (f)

def strip_start_file(lines, start_index):
    global comp_structs
    brace_count = 0
    if (start_index != -1):
        brace_count = lines[start_index].count("{") - lines[start_index].count("}")
    lines = lines[(start_index + 1):]
    new_str = ""
    for l in lines:
        brace_count += l.count("{") - l.count("}")
        if (brace_count == 0 and start_index != -1):
            break
        if (re.findall(r'^struct[ \t]*[a-zA-Z_]*', l)):
            comp_structs.append(l.split()[1])
        elif (re.findall(r'^#[ \t]include[ \t]"[a-zA-Z_]*.[a-z]pp*', l)):
            strip_start_file(read_file(find_file(l.split()[-1][1:-1])), -1)
        new_str += l + '\n'
    return (new_str)


good_files = []
for file in files_list:
    content = read_file(file)
    index = 0
    for line in content:
        if (re.findall(r'^namespace[ \t]*hel::comp', line)):
            good_files.append(strip_start_file(content, index))
            break
        index += 1

output_str = """#include "ecs/ComponentList.hpp"

namespace\thel {

std::vector<const char *>\tComponentList::_componentList{"""

vector_str = ""
for struct in comp_structs:
    vector_str += f"\"{struct}\", "
vector_str = vector_str[:-2] + "};"

output_str += vector_str
output_str += """

void\tComponentList::addComponent(Registry &registry, Entity::id handle, const char *componentName) {\n
"""

for struct in comp_structs:
    output_str += f"\n\tif (componentName == \"{struct}\")\n\t\treturn ((void)registry.addComponent<comp::{struct}>(handle));"

output_str += f"\n\treturn ;\n" + "}\n\n}\n"

output_file = "src/ecs/ComponentList.cpp"
with open(output_file, "w") as f:
    f.write(output_str)
