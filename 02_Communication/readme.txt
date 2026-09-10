README - Communication Practice Diagram
=========================================

File: practice_diagram.drawio

What is this file?
-------------------
This is a flowchart made with draw.io (also called diagrams.net) that shows
how two nodes (devices) exchange CAN messages with each other, including a
checksum and counter check for validating the received data.

How to open it
---------------
1. Go to https://app.diagrams.net (or install the desktop draw.io app).
2. Choose "Open Existing Diagram" and select "practice_diagram.drawio".
3. You can also open it directly in VS Code if you have the
   "Draw.io Integration" extension installed.

What the diagram shows
-----------------------
There are two nodes in the diagram: "Node1" (right side) and "Node 2" (left
side). They send messages back and forth to each other:

1. Node 2 -> Node1: Message 0x0A2
   - data[0] = 1
   - data[1] = 2
   - data[2-5] = 0
   - data[6] = counter
   - data[7] = checksum

2. Node1 receives the message and follows this flow:
   - Read data from the message.
   - Check if checksum and counter are correct (decision/diamond box).
     - If "No" -> Discard the message.
     - If "Yes" -> Print the received data to the terminal, then prepare
       the data to send back:
         send_data[0] = receive_data[0]
         send_data[1] = receive_data[1]
         send_data[2] = receive_data[1] + receive_data[2]
         send_data[3-5] = 0
         send_data[6] = calculated_counter
         send_data[7] = calculated_checksum
   - Print the send data to the terminal, then send the data (this becomes
     message 0x12) back to Node 2.

3. Node1 -> Node 2: Message 0x12
   - data[0] = 1
   - data[1] = 2
   - data[2] = data[1] + data[0] = 3
   - data[3-5] = 0
   - data[6] = counter
   - data[7] = checksum

4. Node 2 receives the reply and repeats a similar flow:
   - Read data from the message.
   - Check if checksum and counter are correct.
     - If "No" -> Discard.
     - If "Yes" -> Print the received data to the terminal, prepare a new
       send message (calculate checksum and counter), print the send data,
       and send it back to Node1.

Purpose / Learning Goal
------------------------
This exercise is meant to help students practice:
- Building and reading CAN message data frames.
- Calculating and validating a checksum and a rolling counter.
- Implementing a request/response (ping-pong) communication loop between
  two nodes.

Tips for students
------------------
- Follow the arrows in order to understand the message flow between the
  two nodes.
- Pay close attention to how the checksum and counter are calculated on
  the sending side and validated on the receiving side - this is the core
  logic you need to implement.
- The diamond (rhombus) shapes are decision points ("Yes"/"No") - make
  sure your code handles both the valid and invalid data cases.
