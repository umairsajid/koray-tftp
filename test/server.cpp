/*
    Y Koray Kalmaz  -   2011
    
    Implementation file for TFTP server object:
    
    TFTP Packets Types:
     1 Read request (RRQ)
     2 Write request (WRQ)
     3 Data (DATA)
     4 Acknowledgment (ACK)
     5 Error (ERROR)
    
    TFTP Operation:
    
    1. Listen for a request:
            WRQ: Write request
        or;
            RRQ: Read  request
        a) Listen port 69 for incomming connections

            RRQ / WRQ Packets:
            
        | OPCODE | FILENAME | 0 | MODE   | 0 |
        |   2    | string   | 1 | string | 1 |
        
        OPCODE:     1 for RRQ and 2 for WRQ
        FILENAME:   ASCII byte sequence for filename, terminated with 0
        MODE:       NETASCII or OCTET or MAIL   (case insensitive)
            NETASCII:   
            OCTET:      
            MAIL:       Similar to NetAscii. Will not be implemented.
        
        
    2. Create a thread for RRQ client IP and Port number
    
    3. Ready data packet:
        Assign block number 1 for the first packet and increment.
        
            DATA Packet:
            
        | OPCODE | Block # | DATA |
        | 2      | 2       | n    |
        
        OPCODE:     3 for DATA
        BLOCK #:    Block number
        DATA:       512 Bytes data fragment. (if 0-512 ==> EOF)
    
    4. Send data packet & wait for its acknowledgement
        In general an acknowledgment packet will contain the block 
        number of the data packet being acknowledged. 
        
        a) Receive ACK:
        
            ACK Packet:
            
        | OPCODE | Block # |
        | 2      | 2       |
        
        OPCODE:     4 for ACK
        BLOCK #:    Related block number
        
        
        b) receive ERROR:
        
            ERROR Packet:
        | OPCODE | ErrCode | ErrMsg | 0 |
        | 2      | 2       | string | 1 |
        
        OPCODE:     5 for ERROR
        ErrorCode:  Integer 
        ErrMSG:     NetAscii human readable message
        0:          Terminated with 0 byte
        
            Error Codes:
        Value   Meaning
        0       Not defined, see error message (if any).
        1       File not found.
        2       Access violation.
        3       Disk full or allocation exceeded.
        4       Illegal TFTP operation.
        5       Unknown transfer ID.
        6       File already exists.
        7       No such user. (Not Implemented)

*/
