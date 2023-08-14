import re
import os

path = "./documentation/html"
content = os.listdir(path)

pattern = re.compile(r"^<li>\[(X| )\] (.+)<\/li>$")

def check(c):
    if c in ['x', 'X']:
        return "checked"
    return ""

index = 1
for item in content:
    if not item.endswith(".html"):
        continue
    
    full_path = os.path.join(path, item)
    # print(f"Processing {full_path}")

    with open(full_path, 'r', encoding='utf-8') as file:
        document = file.read()
        lines = document.split('\n')
    
    with open(full_path, 'w', encoding='utf-8') as file:
        for line in lines:
            match = pattern.search(line)
            if match:
                ok, text = match.groups()
                idx = str(index).zfill(8)
                file.write(f'<li><input id=item_{idx} disabled="disabled" type="checkbox" {check(ok)} /> <label for=item_{idx}>{text}</label></li>\n')
                index += 1
            else:
                file.write(f"{line}\n")
