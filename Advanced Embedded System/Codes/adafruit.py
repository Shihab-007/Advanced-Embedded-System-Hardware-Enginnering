import paho.mqtt.client as mqtt
from Adafruit_IO import *
import time

broker_address = "192.168.82.100"
broker_port = 1883

topic_gas = "sensor/gas"
topic_flame = "sensor/flame"
topic_control = "control"

ADAFRUIT_IO_USERNAME = "chovy96de"
ADAFRUIT_IO_KEY = "aio_DbIt49hzcNtwelmritEnVKmugtTL"
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

def on_message(client, userdata, msg):
    if msg.topic == topic_gas:
        gas_reading = int(msg.payload)
        print("Gas Sensor Reading:", gas_reading)

        # Check if gas reading exceeds 50
        if gas_reading > 50:
            aio.send_data('gas-reading', gas_reading)
            # Send message to 2nd Arduino to turn on LED
            client.publish(topic_control, "LED_ON")
        else:
            aio.send_data('gas-reading', gas_reading)
            # Send message to 2nd Arduino to turn off LED
            client.publish(topic_control, "LED_OFF")
    
    elif msg.topic == topic_flame:
        flame_reading = int(msg.payload)
        print("Flame Sensor Reading:", flame_reading)
        
        # Check if flame reading exceeds 50
        if flame_reading > 50:
            aio.send_data('flame-reading', flame_reading)
            # Send message to 2nd Arduino to turn on buzzer
            client.publish(topic_control, "BUZZER_ON")
        else:
            aio.send_data('flame-reading', flame_reading)
            # Send message to 2nd Arduino to turn off buzzer
            client.publish(topic_control, "BUZZER_OFF")

client = mqtt.Client()
client.on_message = on_message

client.connect(broker_address, broker_port)

client.subscribe(topic_gas)
client.subscribe(topic_flame)

client.loop_start()

# Function to send command to the 2nd Arduino
def send_command_to_arduino(command):
    client.publish(topic_control, command)

# Example usage: Send command to turn on the LED
send_command_to_arduino("LED_ON")

# Example usage: Send command to turn on the buzzer
send_command_to_arduino("BUZZER_ON")

# Delay between each data transmission
transmission_interval = 5  # 5 seconds
transmission_count = 0
transmission_limit = 12  # 12 transmissions within 1 minute

while True:
    client.loop()
    
    # Send data to Adafruit IO
    gas = 100  # Replace with your actual gas sensor reading
    flame = 200  # Replace with your actual flame sensor reading
    aio.send_data('gas-reading', gas)
    aio.send_data('flame-reading', flame)
    
    transmission_count += 1
    
    if transmission_count >= transmission_limit:
        # Pause for 1 minute after reaching the transmission limit
        time.sleep(60)
        transmission_count = 0
    else:
        time.sleep(transmission_interval)

