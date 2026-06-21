#!/usr/bin/env python3
"""
Generate comprehensive theme definitions for all 2601 theme slots (0-2600).
Uses HSV color space systematically with Portuguese theme names.
"""

import colorsys

# Existing theme names (0-2600)
existing_names = {
    0: "Original",
    1: "Vermelho",
    2: "Brasil",
    3: "Noturno",
    4: "Amber",
    5: "Verde",
    6: "Roxo",
    7: "Azul",
    8: "PE5PVB",
    9: "Ciano",
    10: "Vulcao",
    11: "Sakura",
    12: "Oceano",
    13: "Tangerina",
    14: "Carmesim",
    15: "Dendro",
    16: "Monocromo",
    17: "Indigo",
    18: "Queer",
    19: "GoldBrite",
    20: "Bubblegum",
    21: "Larico",
    22: "Esmeralda",
    23: "Artico",
    24: "Crepusculo",
    25: "Turquesa",
    26: "Ametista",
    27: "Ferrugem",
    28: "Neon",
    29: "Prata",
    30: "Aurora",
    31: "Lavanda",
    32: "Caramelo",
    33: "Menta",
    34: "Tomate",
    35: "Safira",
    36: "Amarelo",
    37: "Marinho",
    38: "Carmim",
    39: "Celeste",
    40: "Abacate",
    41: "Magenta",
    42: "Brasa",
    43: "Ardosia",
    44: "Cereja",
    45: "Petroleo",
    46: "Cobalto",
    47: "Mel",
    48: "Vinho",
    49: "Acqua",
    50: "Cobre",
    51: "Algodao",
    52: "Musgo",
    53: "Oxido",
    54: "Manga",
    55: "Uva",
    56: "Maracuja",
    57: "Grafite",
    58: "Champanhe",
    59: "Coral",
    60: "Fantasma",
    61: "Sertao",
    62: "Essence",
    63: "Cyan",
    64: "Crimson",
    65: "Monochrome",
    66: "Volcano",
    67: "Dendro",
    68: "Sakura",
    69: "Whiteout",
    70: "Tangerine",
    71: "Ocean",
    72: "Indigo",
    73: "Queer",
    74: "GoldBrite",
    75: "Bubblegum",
    76: "Essence II",
    77: "777",
    78: "Vinil",
    79: "Ouro",
    80: "Anos 80",
    81: "Plasma",
    82: "Nebulosa",
    83: "Fumaca",
    84: "Arco-Iris",
    85: "Petala",
    86: "Relampago",
}

# Portuguese color/theme descriptors
hue_names = {
    0: "Vermelho",      # Red
    10: "Laranja",      # Orange
    20: "Amarelo",      # Yellow
    30: "Verde Amarelo",# Yellow Green
    40: "Verde",        # Green
    50: "Ciano",        # Cyan
    60: "Azul",         # Blue
    70: "Índigo",       # Indigo
    80: "Roxo",         # Purple
    90: "Magenta",      # Magenta
}

brightness_names = {
    0: "Muito Escuro",
    1: "Escuro",
    2: "Médio",
    3: "Brilhante",
    4: "Muito Brilhante",
}

saturation_names = {
    0: "Dessaturado",
    1: "Baixa Saturação",
    2: "Média Saturação",
    3: "Alta Saturação",
    4: "Totalmente Saturado",
}

def rgb565_to_hex(r, g, b):
    """Convert RGB (0-255) to RGB565 hex."""
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    rgb565 = (r5 << 11) | (g6 << 5) | b5
    return f"0x{rgb565:04X}"

def hsv_to_rgb(h, s, v):
    """Convert HSV (0-360, 0-1, 0-1) to RGB (0-255)."""
    c = colorsys.hsv_to_rgb(h / 360.0, s, v)
    return tuple(int(x * 255) for x in c)

def get_theme_name(index):
    """Get theme name from existing map or generate one."""
    if index in existing_names:
        return existing_names[index]

    # Generate a new name based on HSV
    # Map index to HSV parameters
    if index < 2601:
        # Use 36 hue bands, 5 brightness levels, 15 variants per brightness
        hue_band = (index - 87) // 72  # 36 bands
        brightness_idx = ((index - 87) % 72) // 15  # 5 levels
        variant = (index - 87) % 15  # variant within brightness

        hue = (hue_band * 10) % 360

        # Get closest hue name
        closest_hue = min(hue_names.keys(), key=lambda x: abs(x - hue))
        hue_name = hue_names[closest_hue]

        brightness_name = brightness_names.get(brightness_idx, "Médio")

        return f"{hue_name} {brightness_name} {variant + 1}"

    return "Original"

def generate_theme_colors(index):
    """Generate 6 RGB565 colors for a theme."""
    # Map index to HSV space
    if index == 0:  # Original theme
        return (0x0820, 0x07E0, 0xFD20, 0x07FF, 0x7BEF, 0x07FF)

    # Systematic color generation using HSV
    # 36 hue bands (10 degrees each)
    # 5 brightness levels
    # ~15 variants per band for saturation/combinations

    total_generated = index - 87  # Start generating after existing 87 cases

    if total_generated < 0:
        # For indices 1-86, we have existing themes - need to check
        # Return placeholder colors (these should be filled from existing data)
        return (0x0820, 0x07E0, 0xFD20, 0x07FF, 0x7BEF, 0x07FF)

    # Divide into bands
    hue_band = total_generated // 72  # ~36 bands of 72 colors each
    remaining = total_generated % 72

    brightness_level = remaining // 15  # 5 levels
    saturation_variant = remaining % 15  # 15 variants

    # Calculate HSV
    hue = (hue_band * 10) % 360
    brightness = 0.2 + (brightness_level * 0.15)  # 0.2 to 0.95
    saturation = 0.3 + ((saturation_variant / 15) * 0.7)  # 0.3 to 1.0

    # Generate 6 colors from base HSV
    # Header BG: dark, medium saturation
    header_h, header_s, header_v = hue, saturation * 0.6, brightness * 0.4
    header_rgb = hsv_to_rgb(header_h, header_s, header_v)

    # Freq Color: bright, full saturation
    freq_h, freq_s, freq_v = hue, min(saturation * 1.2, 1.0), brightness * 1.2
    freq_rgb = hsv_to_rgb(freq_h, freq_s, min(freq_v, 1.0))

    # Level Color: adjacent hue, bright
    level_h = (hue + 30) % 360
    level_s, level_v = saturation, brightness
    level_rgb = hsv_to_rgb(level_h, level_s, level_v)

    # Label Color: same as freq or slightly different
    label_h, label_s, label_v = hue, saturation, brightness * 1.1
    label_rgb = hsv_to_rgb(label_h, label_s, min(label_v, 1.0))

    # Dim Color: darker variant
    dim_h, dim_s, dim_v = hue, saturation * 0.7, brightness * 0.5
    dim_rgb = hsv_to_rgb(dim_h, dim_s, dim_v)

    # Border Color: contrasting or accent
    border_h = (hue + 120) % 360
    border_s, border_v = saturation * 0.9, brightness * 0.9
    border_rgb = hsv_to_rgb(border_h, border_s, border_v)

    # Convert to RGB565
    header_hex = rgb565_to_hex(*header_rgb)
    freq_hex = rgb565_to_hex(*freq_rgb)
    level_hex = rgb565_to_hex(*level_rgb)
    label_hex = rgb565_to_hex(*label_rgb)
    dim_hex = rgb565_to_hex(*dim_rgb)
    border_hex = rgb565_to_hex(*border_rgb)

    return (header_hex, freq_hex, level_hex, label_hex, dim_hex, border_hex)

# Generate all 2601 theme names
print("// ===== BLOCK 1: getThemeName cases (all 2601) =====")
print("const char *getThemeName(uint16_t t)")
print("{")
print("  switch (t)")
print("  {")
print("  case 0:")
print('    return "Original";')

for i in range(1, 2601):
    name = get_theme_name(i)
    print(f"  case {i}:")
    print(f'    return "{name}";')

print("  default:")
print('    return "Original";')
print("  }")
print("}")
print()
print()

# Generate all 2601 theme colors
print("// ===== BLOCK 2: applyTheme cases (all 2601) =====")
print("void applyTheme(uint16_t theme)")
print("{")
print("  themeSet = theme % 2601;")
print("  switch (themeSet)")
print("  {")

# Case 0: Original
print("  case 0: // Original — dark navy / cyan")
print("    UI_HEADER_BG = 0x0820;")
print("    UI_FREQ_COLOR = 0x07E0;")
print("    UI_LEVEL_COLOR = 0xFD20;")
print("    UI_LABEL_COLOR = 0x07FF;")
print("    UI_DIM_COLOR = 0x7BEF;")
print("    UI_BORDER_COLOR = 0x07FF;")
print("    break;")

# Generate remaining cases
for i in range(1, 2601):
    colors = generate_theme_colors(i)
    name = get_theme_name(i)

    if i <= 86:  # Existing cases with names
        print(f"  case {i}: // {name}")
    else:
        print(f"  case {i}: // {name}")

    header, freq, level, label, dim, border = colors

    print(f"    UI_HEADER_BG = {header};")
    print(f"    UI_FREQ_COLOR = {freq};")
    print(f"    UI_LEVEL_COLOR = {level};")
    print(f"    UI_LABEL_COLOR = {label};")
    print(f"    UI_DIM_COLOR = {dim};")
    print(f"    UI_BORDER_COLOR = {border};")
    print("    break;")

print("  }")
print("}")
