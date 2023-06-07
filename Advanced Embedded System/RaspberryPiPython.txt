import paho.mqtt.client as mqtt

broker_address = "192.168.109.100"
broker_port = 1883

topic_gas = "sensor/gas"
topic_flame = "sensor/flame"
topic_control = "control"

client = mqtt.Client()

def on_message(client, userdata, msg):
    if msg.topic == topic_gas:
        gas_reading = int(msg.payload)
        print("Gas Sensor Reading:", gas_reading)
        
        # Check if gas reading exceeds 20
        if gas_reading > 20:
            # Send message to 2nd Arduino to turn on LED
            client.publish(topic_control, "LED_ON")
        if gas_reading < 20:
            # Send message to 2nd Arduino to turn on LED
            client.publish(topic_control, "LED_OFF")
    
    elif msg.topic == topic_flame:
        flame_reading = int(msg.payload)
        print("Flame Sensor Reading:", flame_reading)
        
        # Check if flame reading exceeds 20
        if flame_reading > 20:
            # Send message to 2nd Arduino to turn on buzzer
            client.publish(topic_control, "BUZZER_ON")
        if flame_reading < 20:
            # Send message to 2nd Arduino to turn on buzzer
            client.publish(topic_control, "BUZZER_OFF")

client.on_message = on_message

client.connect(broker_address, broker_port)

client.subscribe(topic_gas)
client.subscribe(topic_flame)

# Function to send command to the 2nd Arduino
def send_command_to_arduino(command):
    client.publish(topic_control, command)

client.loop_start()

# Example usage: Send command to turn on the LED
send_command_to_arduino("LED_ON")

# Example usage: Send command to turn on the buzzer
send_command_to_arduino("BUZZER_ON")

