import matplotlib.pyplot as plt
import numpy as np

def plot_data_from_file(filename, output_filename="graph.png"):
    with open(filename, 'r', encoding='utf-8') as f:
        content = f.read()
    
    values_str = content.replace('\n', '').split(';')
    data = []
    for v in values_str:
        v = v.strip()
        if v:
            data.append(float(v))
    
    y = np.array(data)
    x = np.arange(len(y))
    
    mean_val = np.mean(y)
    variance_val = np.var(y, ddof=1)          
    std_val = np.std(y, ddof=1)               
    sem_val = std_val / np.sqrt(len(y))       
    
    print(f"Среднее значение: {mean_val:.4f}")
    print(f"Дисперсия: {variance_val:.4f}")
    print(f"Стандартное отклонение (погрешность): {std_val:.4f}")
    print(f"Стандартная ошибка среднего: {sem_val:.4f}")
    
    plt.figure(figsize=(10, 5))
    plt.plot(x, y, marker='o', linestyle='-', linewidth=2, markersize=4, label='Данные')
    plt.axhline(y=mean_val, color='r', linestyle='--', label=f'Среднее = {mean_val:.2f}')
    plt.fill_between(x, mean_val - std_val, mean_val + std_val, alpha=0.2, color='gray',
                     label=f'±1σ = {std_val:.2f}')
    plt.title("Мандельброт")
    plt.xlabel("Номер цикла")
    plt.ylabel("Количество тиков")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')

plot_data_from_file("plot.data", "plot.png")