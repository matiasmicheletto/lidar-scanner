# Parametros y variables

steps_mm = 32.73    # Conversion de coordenadas a mm
zMax = 2000         # Altura a partir de la cual se truncan los datos
area = 410*430      # Area de medicion

attributes = [      
# Nombre de variable, identificador, tipo
    ("Fecha","date", "str"),
    ("Bloque","block", "str"),
    ("Tratamiento","treatment", "str"),
    ("Orient. siemb.", "orientation", "str"),
    ("Repetición","rep", "str"),
    ("Altura maxima (mm)","max_height", "num"),
    ("Mediana de altura (mm)","median", "num"),
    ("Desvío estándar","std_dev", "num"),
    ("Volumen (L)","volume", "num"),
    ("Entropía modelo","entropy", "num"),
    ("Ruido de sección transv.","sec_noise", "num"),
    ("Materia seca vertical (gr)","ms_v", "num"),
    ("Materia seca horizontal (gr)","ms_h", "num"),
    ("Densidad (gr/L)","density", "num"),
    ("T. línea 0 cm","temp_l_0", "num"),
    ("T. línea 5 cm","temp_l_5", "num"),
    ("T. línea 10 cm","temp_l_10", "num"),
    ("T. surco 0 cm","temp_s_0", "num"),
    ("T. surco 5 cm","temp_s_5", "num"),
    ("T. surco 10 cm","temp_s_10", "num"),
    ("Viento mín. 0 cm","wnd_min_0", "num"),
    ("Viento mín. 30 cm","wnd_min_30", "num"),
    ("Viento mín. 2 m","wnd_min_200", "num"),
    ("Viento prom. 0 cm","wnd_prom_0", "num"),
    ("Viento prom. 30 cm","wnd_prom_30", "num"),
    ("Viento prom. 2 m","wnd_prom_200", "num"),
    ("Viento max. 0 cm","wnd_max_0", "num"),
    ("Viento max. 30 cm","wnd_max_30", "num"),
    ("Viento max. 2 m","wnd_max_200", "num")
]

def get_attribute_name(var):
    index = [n[1] for n in attributes].index(var)
    return attributes[index][0]