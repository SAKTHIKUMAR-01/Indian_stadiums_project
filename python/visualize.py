#!/usr/bin/env python3
"""
Indian Stadium Management System - Python Data Visualization Module
Generates high-resolution data analytics charts from MySQL / CSV data.
"""

import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
import mysql.connector

# Set modern chart styling
plt.style.use('ggplot')
plt.rcParams['font.sans-serif'] = 'DejaVu Sans'
plt.rcParams['axes.edgecolor'] = '#cccccc'
plt.rcParams['axes.linewidth'] = 0.8

def fetch_stadium_data():
    """Fetches stadium dataset from MySQL database or fallback CSV file."""
    try:
        conn = mysql.connector.connect(
            host="127.0.0.1",
            user="root",
            password="",
            database="indian_stadium_db",
            port=3306
        )
        query = "SELECT stadium_id, stadium_name, city, state, capacity, sport, home_team, opened_year, surface FROM stadiums"
        df = pd.read_sql(query, conn)
        conn.close()
        print(f"[Python Viz] Successfully loaded {len(df)} stadium records from MySQL database.")
        return df
    except Exception as e:
        print(f"[Python Viz Warning] Could not connect to MySQL ({e}). Attempting fallback to CSV...")
        csv_path = os.path.join(os.path.dirname(__file__), "..", "data", "stadiums.csv")
        if os.path.exists(csv_path):
            df = pd.read_csv(csv_path)
            print(f"[Python Viz] Loaded {len(df)} records from CSV fallback: {csv_path}")
            return df
        else:
            print("[Python Viz Error] Neither MySQL nor CSV dataset is available.")
            sys.exit(1)

def generate_chart1_stadiums_by_state(df, output_dir):
    """Chart 1: Bar chart showing number of stadiums by state."""
    plt.figure(figsize=(12, 6))
    state_counts = df['state'].value_counts()
    
    bars = plt.bar(state_counts.index, state_counts.values, color='#2b5c8f', edgecolor='#1d3c5e', width=0.6)
    
    plt.title('Number of Stadiums by State in India', fontsize=16, fontweight='bold', pad=15)
    plt.xlabel('State', fontsize=12, labelpad=10)
    plt.ylabel('Number of Stadiums', fontsize=12, labelpad=10)
    plt.xticks(rotation=45, ha='right', fontsize=10)
    plt.yticks(fontsize=10)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    
    # Value annotations on top of bars
    for bar in bars:
        height = bar.get_height()
        plt.annotate(f'{int(height)}',
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=9, fontweight='bold')
        
    plt.tight_layout()
    out_path = os.path.join(output_dir, 'stadiums_by_state.png')
    plt.savefig(out_path, dpi=300)
    plt.close()
    print(f"[Python Viz] Saved Chart 1: {out_path}")

def generate_chart2_sport_distribution(df, output_dir):
    """Chart 2: Pie chart showing distribution of stadiums by primary sport."""
    plt.figure(figsize=(9, 8))
    sport_counts = df['sport'].value_counts()
    
    colors = ['#ff9999', '#66b3ff', '#99ff99', '#ffcc99', '#c2c2f0', '#ffb3e6']
    
    wedges, texts, autotexts = plt.pie(
        sport_counts.values,
        labels=sport_counts.index,
        autopct='%1.1f%%',
        startangle=140,
        colors=colors[:len(sport_counts)],
        shadow=True,
        explode=[0.05] * len(sport_counts),
        textprops=dict(color="black", fontsize=11)
    )
    
    plt.setp(autotexts, size=11, weight="bold")
    plt.title('Distribution of Indian Stadiums by Sport Category', fontsize=16, fontweight='bold', pad=20)
    plt.tight_layout()
    
    out_path = os.path.join(output_dir, 'sport_distribution.png')
    plt.savefig(out_path, dpi=300)
    plt.close()
    print(f"[Python Viz] Saved Chart 2: {out_path}")

def generate_chart3_top_capacity(df, output_dir):
    """Chart 3: Horizontal bar chart showing Top 10 stadiums by seating capacity."""
    plt.figure(figsize=(12, 7))
    
    # Filter valid capacities and get top 10
    top10 = df[df['capacity'] > 0].sort_values(by='capacity', ascending=False).head(10)
    top10 = top10.iloc[::-1]  # Reverse for top-down horizontal bar display
    
    bars = plt.barh(top10['stadium_name'], top10['capacity'] / 1000.0, color='#e056fd', edgecolor='#be2edd', height=0.6)
    
    plt.title('Top 10 Indian Stadiums by Seating Capacity (in Thousands)', fontsize=16, fontweight='bold', pad=15)
    plt.xlabel('Seating Capacity (x1,000)', fontsize=12, labelpad=10)
    plt.ylabel('Stadium Name', fontsize=12, labelpad=10)
    plt.xticks(fontsize=10)
    plt.yticks(fontsize=10)
    plt.grid(axis='x', linestyle='--', alpha=0.7)
    
    for bar in bars:
        width = bar.get_width()
        plt.annotate(f'{width:.1f}k',
                    xy=(width, bar.get_y() + bar.get_height() / 2),
                    xytext=(5, 0),
                    textcoords="offset points",
                    ha='left', va='center', fontsize=9, fontweight='bold')
        
    plt.tight_layout()
    out_path = os.path.join(output_dir, 'capacity_distribution.png')
    plt.savefig(out_path, dpi=300)
    plt.close()
    print(f"[Python Viz] Saved Chart 3: {out_path}")

def main():
    output_dir = os.path.join(os.path.dirname(__file__), "..", "output")
    os.makedirs(output_dir, exist_ok=True)
    
    df = fetch_stadium_data()
    if df.empty:
        print("[Python Viz Error] Stadium dataset is empty.")
        sys.exit(1)
        
    generate_chart1_stadiums_by_state(df, output_dir)
    generate_chart2_sport_distribution(df, output_dir)
    generate_chart3_top_capacity(df, output_dir)
    print("\n[Python Viz SUCCESS] All 3 analytical visualization charts generated successfully!")

if __name__ == '__main__':
    main()
