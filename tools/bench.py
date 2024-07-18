import sys
import time
from uci import UCIEngine

def benchmark_position(engine, position, depth):
    engine.send_command(f'position fen {position}')
    engine.send_command(f'go depth {depth}')
    
    nodes = 0
    start_time = time.time()
    
    while True:
        line = engine.process.stdout.readline().strip()
        if ' nodes ' in line:
            nodes = int(line.split(' nodes ')[1].split()[0])
        if 'bestmove' in line:
            break

    elapsed_time = (time.time() - start_time) * 1000  # convert to milliseconds
    engine.send_command('isready')
    engine.receive_output()
    return nodes, elapsed_time

def main(engine_path, expected_nodes, bench_file, depth=13, hash_size=16):
    # Read the benchmark positions from the file
    with open(bench_file, 'r') as f:
        positions = [line.strip().strip('"') for line in f if line.strip()]

    # Initialize the UCI engine
    engine = UCIEngine(engine_path, hash_size)

    total_nodes = 0
    total_time = 0

    for position in positions:
        nodes, elapsed_time = benchmark_position(engine, position, depth)
        total_time += elapsed_time
        total_nodes += nodes

    engine.close()

    nps = (total_nodes * 1000) / (total_time + 1)  # to avoid division by zero
    print(f'bench: {total_nodes} nodes, nps: {nps:.2f}, time: {total_time:.2f} ms')

    # Check if the actual node count matches the expected node count
    if total_nodes != expected_nodes:
        print(f"Error: Expected {expected_nodes} nodes, but got {total_nodes} nodes.")
        sys.exit(1)  # Exit with failure code

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print(f'Usage: {sys.argv[0]} <engine_path> <expected_nodes> <bench_file> [depth] [hash_size]')
    else:
        engine_path = sys.argv[1]
        expected_nodes = int(sys.argv[2])
        bench_file = sys.argv[3]
        depth = int(sys.argv[4]) if len(sys.argv) > 4 else 13
        hash_size = int(sys.argv[5]) if len(sys.argv) > 5 else 16
        main(engine_path, expected_nodes, bench_file, depth, hash_size)
