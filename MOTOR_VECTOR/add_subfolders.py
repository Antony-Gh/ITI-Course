import re

def add_subfolders(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()

    new_paths = [
        'HAL/LCD', 'HAL/SSD',
        'CONFIG/LCD', 'CONFIG/DIO', 'CONFIG/SSD',
        'MCAL/DIO',
        'HW/DIO', 'HW/LCD', 'HW/SSD'
    ]

    # Find all <option ... name="Include Paths (-I)" ...> ... </option>
    options = re.finditer(r'<option[^>]*name="Include Paths \(-I\)"[^>]*>.*?</option>', content, re.DOTALL)
    
    new_content = content
    offset = 0

    for opt in options:
        opt_block = opt.group(0)
        
        insertion = ""
        for path in new_paths:
            val_str = f'&quot;${{workspace_loc:/${{ProjName}}/{path}}}&quot;'
            if val_str not in opt_block:
                insertion += f'\n\t\t\t\t\t\t\t\t\t<listOptionValue builtIn="false" value="{val_str}"/>'
        
        if insertion:
            # Insert just before the </option> tag
            pos = opt.end() - 9 + offset
            new_content = new_content[:pos] + insertion + new_content[pos:]
            offset += len(insertion)

    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)

add_subfolders('.cproject')
