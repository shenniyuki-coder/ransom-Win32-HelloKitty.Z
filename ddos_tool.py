#!/usr/bin/python3
# -*- coding: utf-8 -*-
# Advanced Distributed Red Team Stress Test Framework
# Author: 32
# Legal lab/testing use only

import asyncio
import aiohttp
import aioredis
import random
import argparse
import sys
import time
from collections import defaultdict

USER_AGENTS = [
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64)",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7)",
    "Mozilla/5.0 (X11; Linux x86_64)",
    "Mozilla/5.0 (iPhone; CPU iPhone OS 13_5 like Mac OS X)",
    "Mozilla/5.0 (iPad; CPU OS 14_0 like Mac OS X)"
]

TASK_QUEUE = "stress_test_tasks"
METRICS_KEY = "stress_test_metrics"

# ----------------------
# Worker Node
# ----------------------
async def worker(name, redis_url, delay):
    redis = await aioredis.from_url(redis_url)
    local_metrics = defaultdict(int)
    async with aiohttp.ClientSession() as session:
        while True:
            task = await redis.lpop(TASK_QUEUE)
            if task is None:
                await asyncio.sleep(delay)
                continue

            url = task.decode()
            headers = {'User-Agent': random.choice(USER_AGENTS)}
            start_time = time.time()
            try:
                async with session.get(url, headers=headers) as resp:
                    elapsed = time.time() - start_time
                    status = resp.status
                    local_metrics['requests'] += 1
                    local_metrics[f'status_{status}'] += 1
                    local_metrics['total_time'] += elapsed
                    print(f"[{name}] {url} -> {status} ({elapsed:.2f}s)")
            except Exception as e:
                local_metrics['errors'] += 1
                print(f"[{name}] Error: {e}")

            # Push metrics to Redis
            await redis.hincrby(METRICS_KEY, f"{name}_requests", 1)
            await redis.hincrby(METRICS_KEY, f"{name}_errors", local_metrics['errors'])

# ----------------------
# Task Feeder
# ----------------------
async def feed_tasks(redis_url, urls, count):
    redis = await aioredis.from_url(redis_url)
    for _ in range(count):
        url = random.choice(urls)
        await redis.rpush(TASK_QUEUE, url)
    print(f"Pushed {count} tasks to {TASK_QUEUE}")

# ----------------------
# Metrics Dashboard
# ----------------------
async def metrics_dashboard(redis_url, interval=5):
    redis = await aioredis.from_url(redis_url)
    while True:
        metrics = await redis.hgetall(METRICS_KEY)
        print("\n--- Metrics Dashboard ---")
        for key, value in metrics.items():
            print(f"{key.decode()}: {value.decode()}")
        print("-------------------------\n")
        await asyncio.sleep(interval)

# ----------------------
# Main Function
# ----------------------
async def main(args):
    if args.mode == "feed":
        urls = args.target.split(",")
        await feed_tasks(args.redis, urls, args.count)
    elif args.mode == "worker":
        tasks = [worker(f"Node-{i+1}", args.redis, args.delay) for i in range(args.nodes)]
        if args.dashboard:
            tasks.append(metrics_dashboard(args.redis, args.interval))
        await asyncio.gather(*tasks)
    else:
        print("Invalid mode. Use 'feed' or 'worker'.")
        sys.exit(1)

# ----------------------
# CLI Arguments
# ----------------------
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advanced Distributed Red Team Stress Test")
    parser.add_argument("-m", "--mode", type=str, required=True, choices=["feed","worker"], help="Mode: feed tasks or run worker")
    parser.add_argument("-r", "--redis", type=str, default="redis://localhost:6379", help="Redis URL")
    parser.add_argument("-t", "--target", type=str, help="Target URL(s), comma-separated for feed mode")
    parser.add_argument("-c", "--count", type=int, default=100, help="Number of tasks to push (feed mode)")
    parser.add_argument("-n", "--nodes", type=int, default=5, help="Number of worker nodes (worker mode)")
    parser.add_argument("-d", "--delay", type=float, default=0.05, help="Delay between task fetches (worker mode)")
    parser.add_argument("--dashboard", action="store_true", help="Enable live metrics dashboard")
    parser.add_argument("--interval", type=int, default=5, help="Dashboard update interval (seconds)")
    args = parser.parse_args()

    if args.mode == "feed" and not args.target:
        print("Target URL(s) required for feed mode")
        sys.exit(1)

    asyncio.run(main(args))
