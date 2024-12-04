import re

res = 0

with open("d3/input.txt", "r") as file:
    data = file.read()
    pattern = re.compile(r"(?<=mul)\(\d+,\d+\)")
    matches = pattern.findall(data)

    for match in matches:
        str = match.replace("(", "").replace(")", "")
        a, b = str.split(",")
        res += int(a) * int(b)

print(res)
