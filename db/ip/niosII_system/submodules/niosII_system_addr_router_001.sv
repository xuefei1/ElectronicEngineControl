// (C) 2001-2013 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License Subscription 
// Agreement, Altera MegaCore Function License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the applicable 
// agreement for further details.


// $Id: //acds/rel/12.1sp1/ip/merlin/altera_merlin_router/altera_merlin_router.sv.terp#1 $
// $Revision: #1 $
// $Date: 2012/10/10 $
// $Author: swbranch $

// -------------------------------------------------------
// Merlin Router
//
// Asserts the appropriate one-hot encoded channel based on 
// either (a) the address or (b) the dest id. The DECODER_TYPE
// parameter controls this behaviour. 0 means address decoder,
// 1 means dest id decoder.
//
// In the case of (a), it also sets the destination id.
// -------------------------------------------------------

`timescale 1 ns / 1 ns

module niosII_system_addr_router_001_default_decode
  #(
     parameter DEFAULT_CHANNEL = 2,
               DEFAULT_DESTID = 2 
   )
  (output [93 - 89 : 0] default_destination_id,
   output [20-1 : 0] default_src_channel
  );

  assign default_destination_id = 
    DEFAULT_DESTID[93 - 89 : 0];
  generate begin : default_decode
    if (DEFAULT_CHANNEL == -1)
      assign default_src_channel = '0;
    else
      assign default_src_channel = 20'b1 << DEFAULT_CHANNEL;
  end
  endgenerate

endmodule


module niosII_system_addr_router_001
(
    // -------------------
    // Clock & Reset
    // -------------------
    input clk,
    input reset,

    // -------------------
    // Command Sink (Input)
    // -------------------
    input                       sink_valid,
    input  [104-1 : 0]    sink_data,
    input                       sink_startofpacket,
    input                       sink_endofpacket,
    output                      sink_ready,

    // -------------------
    // Command Source (Output)
    // -------------------
    output                          src_valid,
    output reg [104-1    : 0] src_data,
    output reg [20-1 : 0] src_channel,
    output                          src_startofpacket,
    output                          src_endofpacket,
    input                           src_ready
);

    // -------------------------------------------------------
    // Local parameters and variables
    // -------------------------------------------------------
    localparam PKT_ADDR_H = 62;
    localparam PKT_ADDR_L = 36;
    localparam PKT_DEST_ID_H = 93;
    localparam PKT_DEST_ID_L = 89;
    localparam ST_DATA_W = 104;
    localparam ST_CHANNEL_W = 20;
    localparam DECODER_TYPE = 0;

    localparam PKT_TRANS_WRITE = 65;
    localparam PKT_TRANS_READ  = 66;

    localparam PKT_ADDR_W = PKT_ADDR_H-PKT_ADDR_L + 1;
    localparam PKT_DEST_ID_W = PKT_DEST_ID_H-PKT_DEST_ID_L + 1;




    // -------------------------------------------------------
    // Figure out the number of bits to mask off for each slave span
    // during address decoding
    // -------------------------------------------------------
    localparam PAD0 = log2ceil(64'h4000000 - 64'h2000000);
    localparam PAD1 = log2ceil(64'h4008000 - 64'h4004000);
    localparam PAD2 = log2ceil(64'h4009000 - 64'h4008800);
    localparam PAD3 = log2ceil(64'h4009020 - 64'h4009000);
    localparam PAD4 = log2ceil(64'h4009040 - 64'h4009020);
    localparam PAD5 = log2ceil(64'h4009060 - 64'h4009040);
    localparam PAD6 = log2ceil(64'h4009080 - 64'h4009060);
    localparam PAD7 = log2ceil(64'h4009090 - 64'h4009080);
    localparam PAD8 = log2ceil(64'h40090a0 - 64'h4009090);
    localparam PAD9 = log2ceil(64'h40090b0 - 64'h40090a0);
    localparam PAD10 = log2ceil(64'h40090c0 - 64'h40090b0);
    localparam PAD11 = log2ceil(64'h40090c8 - 64'h40090c0);
    localparam PAD12 = log2ceil(64'h40090d0 - 64'h40090c8);
    localparam PAD13 = log2ceil(64'h40090d8 - 64'h40090d0);
    localparam PAD14 = log2ceil(64'h40090dc - 64'h40090d8);
    localparam PAD15 = log2ceil(64'h40090e0 - 64'h40090dc);
    localparam PAD16 = log2ceil(64'h40090e4 - 64'h40090e0);
    localparam PAD17 = log2ceil(64'h40090e8 - 64'h40090e4);
    localparam PAD18 = log2ceil(64'h40090e9 - 64'h40090e8);
    localparam PAD19 = log2ceil(64'h40090ea - 64'h40090e9);
    // -------------------------------------------------------
    // Work out which address bits are significant based on the
    // address range of the slaves. If the required width is too
    // large or too small, we use the address field width instead.
    // -------------------------------------------------------
    localparam ADDR_RANGE = 64'h40090ea;
    localparam RANGE_ADDR_WIDTH = log2ceil(ADDR_RANGE);
    localparam OPTIMIZED_ADDR_H = (RANGE_ADDR_WIDTH > PKT_ADDR_W) ||
                                  (RANGE_ADDR_WIDTH == 0) ?
                                        PKT_ADDR_H :
                                        PKT_ADDR_L + RANGE_ADDR_WIDTH - 1;
    localparam RG = RANGE_ADDR_WIDTH-1;

      wire [PKT_ADDR_W-1 : 0] address = sink_data[OPTIMIZED_ADDR_H : PKT_ADDR_L];

    // -------------------------------------------------------
    // Pass almost everything through, untouched
    // -------------------------------------------------------
    assign sink_ready        = src_ready;
    assign src_valid         = sink_valid;
    assign src_startofpacket = sink_startofpacket;
    assign src_endofpacket   = sink_endofpacket;

    wire [PKT_DEST_ID_W-1:0] default_destid;
    wire [20-1 : 0] default_src_channel;




    niosII_system_addr_router_001_default_decode the_default_decode(
      .default_destination_id (default_destid),
      .default_src_channel (default_src_channel)
    );

    always @* begin
        src_data    = sink_data;
        src_channel = default_src_channel;

        src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = default_destid;
        // --------------------------------------------------
        // Address Decoder
        // Sets the channel and destination ID based on the address
        // --------------------------------------------------

        // ( 0x2000000 .. 0x4000000 )
        if ( {address[RG:PAD0],{PAD0{1'b0}}} == 27'h2000000 ) begin
            src_channel = 20'b00000000000000000100;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 2;
        end

        // ( 0x4004000 .. 0x4008000 )
        if ( {address[RG:PAD1],{PAD1{1'b0}}} == 27'h4004000 ) begin
            src_channel = 20'b00000000000000000010;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 1;
        end

        // ( 0x4008800 .. 0x4009000 )
        if ( {address[RG:PAD2],{PAD2{1'b0}}} == 27'h4008800 ) begin
            src_channel = 20'b00000000000000000001;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 0;
        end

        // ( 0x4009000 .. 0x4009020 )
        if ( {address[RG:PAD3],{PAD3{1'b0}}} == 27'h4009000 ) begin
            src_channel = 20'b00001000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 15;
        end

        // ( 0x4009020 .. 0x4009040 )
        if ( {address[RG:PAD4],{PAD4{1'b0}}} == 27'h4009020 ) begin
            src_channel = 20'b00000000100000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 11;
        end

        // ( 0x4009040 .. 0x4009060 )
        if ( {address[RG:PAD5],{PAD5{1'b0}}} == 27'h4009040 ) begin
            src_channel = 20'b00000000001000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 9;
        end

        // ( 0x4009060 .. 0x4009080 )
        if ( {address[RG:PAD6],{PAD6{1'b0}}} == 27'h4009060 ) begin
            src_channel = 20'b00000000000000010000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 4;
        end

        // ( 0x4009080 .. 0x4009090 )
        if ( {address[RG:PAD7],{PAD7{1'b0}}} == 27'h4009080 ) begin
            src_channel = 20'b00000100000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 14;
        end

        // ( 0x4009090 .. 0x40090a0 )
        if ( {address[RG:PAD8],{PAD8{1'b0}}} == 27'h4009090 ) begin
            src_channel = 20'b00000000000100000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 8;
        end

        // ( 0x40090a0 .. 0x40090b0 )
        if ( {address[RG:PAD9],{PAD9{1'b0}}} == 27'h40090a0 ) begin
            src_channel = 20'b00000000000010000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 7;
        end

        // ( 0x40090b0 .. 0x40090c0 )
        if ( {address[RG:PAD10],{PAD10{1'b0}}} == 27'h40090b0 ) begin
            src_channel = 20'b00000000000000100000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 5;
        end

        // ( 0x40090c0 .. 0x40090c8 )
        if ( {address[RG:PAD11],{PAD11{1'b0}}} == 27'h40090c0 ) begin
            src_channel = 20'b00000000010000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 10;
        end

        // ( 0x40090c8 .. 0x40090d0 )
        if ( {address[RG:PAD12],{PAD12{1'b0}}} == 27'h40090c8 ) begin
            src_channel = 20'b00000000000001000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 6;
        end

        // ( 0x40090d0 .. 0x40090d8 )
        if ( {address[RG:PAD13],{PAD13{1'b0}}} == 27'h40090d0 ) begin
            src_channel = 20'b00000000000000001000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 3;
        end

        // ( 0x40090d8 .. 0x40090dc )
        if ( {address[RG:PAD14],{PAD14{1'b0}}} == 27'h40090d8 ) begin
            src_channel = 20'b01000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 18;
        end

        // ( 0x40090dc .. 0x40090e0 )
        if ( {address[RG:PAD15],{PAD15{1'b0}}} == 27'h40090dc ) begin
            src_channel = 20'b00100000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 17;
        end

        // ( 0x40090e0 .. 0x40090e4 )
        if ( {address[RG:PAD16],{PAD16{1'b0}}} == 27'h40090e0 ) begin
            src_channel = 20'b00000010000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 13;
        end

        // ( 0x40090e4 .. 0x40090e8 )
        if ( {address[RG:PAD17],{PAD17{1'b0}}} == 27'h40090e4 ) begin
            src_channel = 20'b00000001000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 12;
        end

        // ( 0x40090e8 .. 0x40090e9 )
        if ( {address[RG:PAD18],{PAD18{1'b0}}} == 27'h40090e8 ) begin
            src_channel = 20'b10000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 19;
        end

        // ( 0x40090e9 .. 0x40090ea )
        if ( {address[RG:PAD19],{PAD19{1'b0}}} == 27'h40090e9 ) begin
            src_channel = 20'b00010000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 16;
        end

end


    // --------------------------------------------------
    // Ceil(log2()) function
    // --------------------------------------------------
    function integer log2ceil;
        input reg[65:0] val;
        reg [65:0] i;

        begin
            i = 1;
            log2ceil = 0;

            while (i < val) begin
                log2ceil = log2ceil + 1;
                i = i << 1;
            end
        end
    endfunction

endmodule


