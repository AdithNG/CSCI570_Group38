import os
import subprocess
import matplotlib.pyplot as plt

INPUT_DIR = "inputs"
OUTPUT_DIR = "outputs"

BASIC_SCRIPT = "./basic.sh"
EFF_SCRIPT = "./efficient.sh"

print(f"{'M+N':<8} {'Time_basic(ms)':<15} {'Mem_basic(KB)':<15} "
      f"{'Time_eff(ms)':<15} {'Mem_eff(KB)':<15}")

os.makedirs(OUTPUT_DIR, exist_ok=True)

MN_list = []
time_basic_list = []
time_eff_list = []
mem_basic_list = []
mem_eff_list = []

for fname in sorted(os.listdir(INPUT_DIR)):
    if not fname.endswith(".txt"):
        continue

    input_path = os.path.join(INPUT_DIR, fname)
    base_out = os.path.join(OUTPUT_DIR, "basic_" + fname)
    eff_out  = os.path.join(OUTPUT_DIR, "eff_" + fname)

    subprocess.run([BASIC_SCRIPT, input_path, base_out], stdout=subprocess.DEVNULL)

    subprocess.run([EFF_SCRIPT, input_path, eff_out], stdout=subprocess.DEVNULL)

    with open(base_out, "r") as f:
        lines = [line.strip() for line in f.readlines()]
        cost_b = lines[0]
        alignX_b = lines[1]
        alignY_b = lines[2]
        time_b = float(lines[3])
        mem_b = float(lines[4])
        M = len(alignX_b.replace("_", ""))
        N = len(alignY_b.replace("_", ""))
        MN = M + N

    with open(eff_out, "r") as f:
        lines = [line.strip() for line in f.readlines()]
        cost_e = lines[0]
        alignX_e = lines[1]
        alignY_e = lines[2]
        time_e = float(lines[3])
        mem_e = float(lines[4])

    print(f"{MN:<8} {time_b:<15.2f} {mem_b:<15.2f} "
          f"{time_e:<15.2f} {mem_e:<15.2f}")

    MN_list.append(MN)
    time_basic_list.append(time_b)
    time_eff_list.append(time_e)
    mem_basic_list.append(mem_b)
    mem_eff_list.append(mem_e)

plt.figure(figsize=(10,6))
plt.plot(MN_list, time_basic_list, marker='o', label="Basic")
plt.plot(MN_list, time_eff_list, marker='o', label="Efficient")
plt.xlabel("M + N (final sequence lengths)")
plt.ylabel("Time (ms)")
plt.title("Running Time vs (M+N)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("time_plot.png")
plt.close()

plt.figure(figsize=(10,6))
plt.plot(MN_list, mem_basic_list, marker='o', label="Basic")
plt.plot(MN_list, mem_eff_list, marker='o', label="Efficient")
plt.xlabel("M + N (final sequence lengths)")
plt.ylabel("Memory (KB)")
plt.title("Memory Usage vs (M+N)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("memory_plot.png")
plt.close()

print("\nGenerated plots:")
print(" - time_plot.png")
print(" - memory_plot.png")