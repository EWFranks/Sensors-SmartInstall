# -*- coding: utf-8 -*-
"""
Created on Mon Jul 24 11:21:15 2023

@author: hyuga
"""

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import matplotlib.image as mpimg

def plot_room_with_overlay(room_width, room_height, motion_zones, image_path):
    fig, ax = plt.subplots()

    # Load the image to overlay
    img = mpimg.imread(image_path)
    ax.imshow(img, extent=[0, room_width, 0, room_height], alpha=0.5)

    # Plot the room blueprint grid (Removed as the blueprint had a good grid already)
    #for x in range(room_width + 1):
    #    ax.plot([x, x], [0, room_height], color='gray', linestyle='-', linewidth=0.5)
    #for y in range(room_height + 1):
    #    ax.plot([0, room_width], [y, y], color='gray', linestyle='-', linewidth=0.5)

    # Plot the motion detection zones as partially transparent polygons
    for zone, color in motion_zones:
        if isinstance(zone, Polygon):
            ax.add_patch(zone)
            zone.set_facecolor(color)
            zone.set_alpha(0.5)
        else:
            x, y, width, height = zone
            ax.add_patch(plt.Rectangle((x, y), width, height, color=color, alpha=0.5))

    ax.set_aspect('equal')
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Room Blueprint with Motion Detection Zones and Image Overlay')
    plt.grid(True)
    plt.show()

# Example usage:
if __name__ == "__main__":
    # Room blueprint dimensions
    room_width = 30
    room_height = 20

    # Motion detection zones defined as (shape, color)
    motion_zones = {
        (Polygon([(10, 20),(15, 3), (7, 3), (7, 20)], closed=True), 'purple'),  #Zone 1
        (Polygon([(17, 3),(15, 3), (10, 20), (22, 20)], closed=True), 'red'),   # Zone 2
        (Polygon([(24, 3),(26,3), (30, 20), (20, 20)], closed=True), 'blue'),  # Zone 3
        (Polygon([(26, 11),(26, 10), (7, 5), (7, 16)], closed=True), 'green'),  # Zone A
        (Polygon([(26, 14),(26, 13), (7, 7), (7, 20)], closed=True), 'yellow')  # Zone B
    }

    # Path to the image file for overlay
    image_path = 'C:/Users/hyuga/OneDrive/Documents/Research/Sensors/PIR Demo/2015_Blueprint.PNG'

    zone_motion_status = [True, False, True, True, False]

    plot_room_with_overlay(room_width, room_height, motion_zones, image_path)
