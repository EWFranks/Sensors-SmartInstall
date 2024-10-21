# -*- coding: utf-8 -*-
"""
Created on Tue Jul 25 12:27:55 2023

@author: hyuga

First attempt at creating an app dashboard

"""

from flask import Flask, render_template
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app)

@app.route('/')
def index():
    # Your code to generate the motion_zones and other variables
    # ...

    return render_template('dashboard.html', room_width=room_width, room_height=room_height, motion_zones=motion_zones)

@app.route('/update_motion', methods=['POST'])
def update_motion():
    # Process MQTT message and update motion_zones data
    # ...

    # Emit the updated motion_zones data to all connected clients (frontends)
    socketio.emit('motion_update', motion_zones, broadcast=True)

    return jsonify({'message': 'Motion data updated successfully.'})