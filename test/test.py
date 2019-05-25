import time
import ttn
import datetime

app_id = "you_app_id"
access_key = "you_access_key"

print('Time\t Dev Id\t\tCNT\tFrec\tData Rate\tPW  CH  DR  TR')

def uplink_callback(msg, client):
  now = datetime.datetime.now()
  print (now.strftime("%H:%M:%S"), end = ' ')
  print(msg.dev_id, end = '\t')
  print(msg.counter, end = '\t')
  print(msg.metadata.frequency, end = '\t')
  print(msg.metadata.data_rate, end = '\t')
  print(msg.payload_fields.Power, end = '   ')
  print(msg.payload_fields.chan, end = '   ')
  print(msg.payload_fields.rate, end = '   ')
  print(msg.payload_fields.tried)


handler = ttn.HandlerClient(app_id, access_key,"discovery.thethings.network:1900")

# using mqtt client
mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()


while True:
    time.sleep(30)

mqtt_client.close()
