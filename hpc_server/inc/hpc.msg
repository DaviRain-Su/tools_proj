syntax = "proto3";
package hpc;

//simple_cmd_reply
message simple_cmd_reply{
    uint32 preamble = 1;
    uint32 len = 2;
    uint32 version = 3;
    uint32 total_package_len = 4;
    uint32 seq = 5;
    uint32 flag = 6;
    uint32 cmd = 7;
    uint32 rtcode  = 8;
}

//simple_cmd_reply_data
message simple_cmd_reply_data {
      uint32 preamble = 1;
      uint32 len = 2;
      uint32 version = 3;
      uint32 total_package_len = 4;
      uint32 seq = 5;
      uint32 flag = 6;
      uint32 cmd = 7;
      uint32 rtcode  = 8;
      string databuf = 9;
}

// cmd_begin_reply
message cmd_begin_reply {
        uint32 preamble = 1;
        uint32 len = 2;
        uint32 version = 3;
        uint32 total_package_len = 4;
        uint32 seq = 5;
        uint32 flag = 6;
        uint32 cmd = 7;
        uint64 base_address = 8;
        string interrupt_path = 9;
}

// cmd_tx_data_reply
message cmd_tx_data_reply {
        uint32 preamble = 1;
        uint32 len = 2;
        uint32 version = 3;
        uint32 total_package_len = 4;
        uint32 seq = 5;
        uint32 flag = 6;
        uint32 cmd = 7;
        uint64 input_address = 8;
}

//cmd_get_data_reply
message cmd_get_data_reply {
        uint32 preamble = 1;
        uint32 len = 2;
        uint32 version = 3;
        uint32 total_package_len = 4;
        uint32 seq = 5;
        uint32 flag = 6;
        uint32 cmd = 7;
        string data = 8;
}

//cmd_config_reply
message cmd_config_reply {
        uint32 preamble = 1;
        uint32 len = 2;
        uint32 version = 3;
        uint32 total_package_len = 4;
        uint32 seq = 5;
        uint32 flag = 6;
        uint32 cmd = 7;
        string config = 8;
}

//cmd_test_reg_reply
message cmd_test_reg_reply {
        uint32 preamble = 1;
        uint32 len = 2;
        uint32 version = 3;
        uint32 total_package_len = 4;
        uint32 seq = 5;
        uint32 flag = 6;
        uint32 cmd = 7;
        string reg = 8;
}
