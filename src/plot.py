import matplotlib.pyplot as plt

def plot_data_from_file(filename, output_filename="graph.png"):
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.read()
    
    values_str = content.replace('\n', '').split(';')
    data = []
    for v in values_str:
        v = v.strip()
        if v:
            data.append(float(v))
    
    x = list(range(len(data)))
    y = data
    
    plt.figure(figsize=(10, 5))
    plt.plot(x, y, marker='o', linestyle='-', linewidth=2, markersize=4)
    plt.title("Мандельброт")
    plt.xlabel("Номер цикла")
    plt.ylabel("Количество тиков")
    plt.grid(True)
    plt.tight_layout()
    
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')


plot_data_from_file("plot.data", "plot.png")