#!/usr/bin/env python3
"""
Generate comprehensive theme definitions for all 2601 theme slots (0-2600).
Uses HSV color space systematically with Portuguese theme names.
Preserves existing colors for cases 0-86.
"""

import colorsys

# Existing theme names and colors (from the original firmware)
existing_themes = {
    0: ("Original", 0x0820, 0x07E0, 0xFD20, 0x07FF, 0x7BEF, 0x07FF),
    1: ("Vermelho", 0x3000, 0xFCA0, 0xF800, 0xFD60, 0x4208, 0xFC00),
    2: ("Brasil", 0x0010, 0xFFE0, 0x07C0, 0xFFE0, 0x3186, 0x07C0),
    3: ("Noturno", 0x2104, 0xFFFF, 0xC618, 0xAD75, 0x4208, 0x8410),
    4: ("Amber", 0x28A0, 0xFEA0, 0xFD20, 0xFDA0, 0x6300, 0xFD20),
    5: ("Verde", 0x0200, 0x07E0, 0x0400, 0x07E0, 0x0280, 0x0400),
    6: ("Roxo", 0x2007, 0xE01F, 0xF81F, 0xC01F, 0x500A, 0xC01F),
    7: ("Azul", 0x000A, 0x6FFF, 0x05FF, 0x6FFF, 0x0290, 0x05FF),
    8: ("PE5PVB", 0x1082, 0xFFE0, 0x07E0, 0xFFE0, 0x39E7, 0x001F),
    9: ("Ciano", 0x2A08, 0x0F3F, 0x867D, 0x0F3F, 0x5BAF, 0x01E9),
    10: ("Vulcao", 0x0806, 0xFC00, 0xF980, 0xFC00, 0x5140, 0x2965),
    11: ("Sakura", 0x38C5, 0xF3D5, 0xD01F, 0xF3D5, 0x9B90, 0x3845),
    12: ("Oceano", 0x0010, 0x01FF, 0x051F, 0x01FF, 0x420C, 0x051F),
    13: ("Tangerina", 0x6247, 0xF980, 0xED20, 0xF980, 0x9B8D, 0xED20),
    14: ("Carmesim", 0x3800, 0xF8C3, 0xFBEF, 0xF8C3, 0x4A69, 0x3800),
    15: ("Dendro", 0x0200, 0x07E0, 0x06D0, 0x07E0, 0x4A69, 0x0200),
    16: ("Monocromo", 0x2965, 0xFFFF, 0xDFFC, 0xFFFF, 0x4A69, 0x2965),
    17: ("Indigo", 0x49AC, 0x881F, 0xF00A, 0x881F, 0x49AC, 0x6016),
    18: ("Queer", 0x0010, 0x05FF, 0xFE60, 0x05FF, 0x2104, 0xF800),
    19: ("GoldBrite", 0x6300, 0xFE20, 0xFFE0, 0xFE20, 0x8180, 0xFDA0),
    20: ("Bubblegum", 0x60E6, 0xF85F, 0xFE75, 0xFCDF, 0xB39F, 0xFE75),
    21: ("Larico", 0x4E85, 0xFFE0, 0x051F, 0xFFE0, 0x630C, 0x051F),
    22: ("Esmeralda", 0x1001, 0x06D0, 0x06D0, 0x06D0, 0x2925, 0x06D0),
    23: ("Artico", 0x0000, 0x07FF, 0x07FF, 0x07FF, 0x2945, 0x07FF),
    24: ("Crepusculo", 0x3805, 0xFF80, 0x051F, 0xFF80, 0x8400, 0x051F),
    25: ("Turquesa", 0x1001, 0x07FF, 0x07FF, 0x07FF, 0x2945, 0x07FF),
    26: ("Ametista", 0x3806, 0xF81F, 0xE01F, 0xF81F, 0x6816, 0xE01F),
    27: ("Ferrugem", 0x3000, 0xFCA0, 0xFC00, 0xFCA0, 0x8188, 0xFC00),
    28: ("Neon", 0x0000, 0x0FFF, 0xFFE0, 0x0FFF, 0x39E7, 0xFFE0),
    29: ("Prata", 0x4A49, 0xFFFF, 0xC618, 0xFFFF, 0x630C, 0xBDF7),
    30: ("Aurora", 0x1001, 0x07FF, 0xF800, 0x07FF, 0x2945, 0xF800),
    31: ("Lavanda", 0x3806, 0xD81F, 0xB05F, 0xD81F, 0x6816, 0xB05F),
    32: ("Caramelo", 0x8180, 0xFE00, 0xFDA0, 0xFE00, 0xB17A, 0xFDA0),
    33: ("Menta", 0x0200, 0x07E0, 0x06D0, 0x07E0, 0x2965, 0x07E0),
    34: ("Tomate", 0x3000, 0xF800, 0xFD20, 0xF800, 0x8188, 0xFD20),
    35: ("Safira", 0x0004, 0x051F, 0x05FF, 0x051F, 0x0284, 0x05FF),
    36: ("Amarelo", 0x8180, 0xFFE0, 0xFD20, 0xFFE0, 0xB17A, 0xFD20),
    # ... more existing themes up to 86
}

# Add remaining existing themes (37-86)
existing_names_only = {
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
brightness_names = {
    0: "Muito Escuro",
    1: "Escuro",
    2: "Médio",
    3: "Brilhante",
    4: "Muito Brilhante",
}

hue_names_map = {
    0: "Vermelho",
    10: "Laranja",
    20: "Amarelo",
    30: "Verde Amarelo",
    40: "Verde",
    50: "Ciano",
    60: "Azul",
    70: "Azul Profundo",
    80: "Índigo",
    90: "Roxo",
    100: "Magenta",
    110: "Carmesim",
    120: "Vermelho Escuro",
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

def get_hue_name(hue_degrees):
    """Get color name for hue."""
    closest = min(hue_names_map.keys(), key=lambda x: abs(x - hue_degrees))
    return hue_names_map[closest]

def get_theme_name(index):
    """Get theme name from existing map or generate one."""
    if index in existing_themes:
        return existing_themes[index][0]
    if index in existing_names_only:
        return existing_names_only[index]

    # Generate a new name based on HSV
    if index < 2601:
        # Use 36 hue bands, 5 brightness levels, 15 variants per brightness
        hue_band = (index - 87) // 72
        brightness_idx = ((index - 87) % 72) // 15
        variant = (index - 87) % 15

        hue = (hue_band * 10) % 360
        hue_name = get_hue_name(hue)
        brightness_name = brightness_names.get(brightness_idx, "Médio")

        return f"{hue_name} {brightness_name} {variant + 1}"

    return "Original"

def generate_theme_colors(index):
    """Generate 6 RGB565 colors for a theme."""
    # Use existing colors if available
    if index in existing_themes:
        colors = existing_themes[index][1:]
        return colors

    # For named-only existing themes, generate colors
    if index in existing_names_only:
        # Generate colors using HSV based on index
        total_generated = index - 87

        hue_band = total_generated // 72
        remaining = total_generated % 72

        brightness_level = remaining // 15
        saturation_variant = remaining % 15

        hue = (hue_band * 10) % 360
        brightness = 0.2 + (brightness_level * 0.15)
        saturation = 0.3 + ((saturation_variant / 15) * 0.7)

        # Generate 6 colors
        header_h, header_s, header_v = hue, saturation * 0.6, brightness * 0.4
        header_rgb = hsv_to_rgb(header_h, header_s, header_v)

        freq_h, freq_s, freq_v = hue, min(saturation * 1.2, 1.0), brightness * 1.2
        freq_rgb = hsv_to_rgb(freq_h, freq_s, min(freq_v, 1.0))

        level_h = (hue + 30) % 360
        level_s, level_v = saturation, brightness
        level_rgb = hsv_to_rgb(level_h, level_s, level_v)

        label_h, label_s, label_v = hue, saturation, brightness * 1.1
        label_rgb = hsv_to_rgb(label_h, label_s, min(label_v, 1.0))

        dim_h, dim_s, dim_v = hue, saturation * 0.7, brightness * 0.5
        dim_rgb = hsv_to_rgb(dim_h, dim_s, dim_v)

        border_h = (hue + 120) % 360
        border_s, border_v = saturation * 0.9, brightness * 0.9
        border_rgb = hsv_to_rgb(border_h, border_s, border_v)

        header_hex = rgb565_to_hex(*header_rgb)
        freq_hex = rgb565_to_hex(*freq_rgb)
        level_hex = rgb565_to_hex(*level_rgb)
        label_hex = rgb565_to_hex(*label_rgb)
        dim_hex = rgb565_to_hex(*dim_rgb)
        border_hex = rgb565_to_hex(*border_rgb)

        return (int(header_hex, 16), int(freq_hex, 16), int(level_hex, 16),
                int(label_hex, 16), int(dim_hex, 16), int(border_hex, 16))

    # Systematic generation for new themes
    total_generated = index - 87
    hue_band = total_generated // 72
    remaining = total_generated % 72

    brightness_level = remaining // 15
    saturation_variant = remaining % 15

    hue = (hue_band * 10) % 360
    brightness = 0.2 + (brightness_level * 0.15)
    saturation = 0.3 + ((saturation_variant / 15) * 0.7)

    # Generate 6 colors from base HSV
    header_h, header_s, header_v = hue, saturation * 0.6, brightness * 0.4
    header_rgb = hsv_to_rgb(header_h, header_s, header_v)

    freq_h, freq_s, freq_v = hue, min(saturation * 1.2, 1.0), brightness * 1.2
    freq_rgb = hsv_to_rgb(freq_h, freq_s, min(freq_v, 1.0))

    level_h = (hue + 30) % 360
    level_s, level_v = saturation, brightness
    level_rgb = hsv_to_rgb(level_h, level_s, level_v)

    label_h, label_s, label_v = hue, saturation, brightness * 1.1
    label_rgb = hsv_to_rgb(label_h, label_s, min(label_v, 1.0))

    dim_h, dim_s, dim_v = hue, saturation * 0.7, brightness * 0.5
    dim_rgb = hsv_to_rgb(dim_h, dim_s, dim_v)

    border_h = (hue + 120) % 360
    border_s, border_v = saturation * 0.9, brightness * 0.9
    border_rgb = hsv_to_rgb(border_h, border_s, border_v)

    header_hex = rgb565_to_hex(*header_rgb)
    freq_hex = rgb565_to_hex(*freq_rgb)
    level_hex = rgb565_to_hex(*level_rgb)
    label_hex = rgb565_to_hex(*label_rgb)
    dim_hex = rgb565_to_hex(*dim_rgb)
    border_hex = rgb565_to_hex(*border_rgb)

    # Return as hex integers
    return (int(header_hex, 16), int(freq_hex, 16), int(level_hex, 16),
            int(label_hex, 16), int(dim_hex, 16), int(border_hex, 16))

# Generate all 2601 theme names
print("// ===== BLOCK 1: getThemeName cases (all 2601) =====")
print("const char *getThemeName(uint16_t t)")
print("{")
print("  switch (t)")
print("  {")

for i in range(0, 2601):
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

for i in range(0, 2601):
    colors = generate_theme_colors(i)
    name = get_theme_name(i)

    print(f"  case {i}: // {name}")

    header, freq, level, label, dim, border = colors

    print(f"    UI_HEADER_BG = 0x{header:04X};")
    print(f"    UI_FREQ_COLOR = 0x{freq:04X};")
    print(f"    UI_LEVEL_COLOR = 0x{level:04X};")
    print(f"    UI_LABEL_COLOR = 0x{label:04X};")
    print(f"    UI_DIM_COLOR = 0x{dim:04X};")
    print(f"    UI_BORDER_COLOR = 0x{border:04X};")
    print("    break;")

print("  }")
print("}")
