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
  (output [95 - 90 : 0] default_destination_id,
   output [34-1 : 0] default_src_channel
  );

  assign default_destination_id = 
    DEFAULT_DESTID[95 - 90 : 0];
  generate begin : default_decode
    if (DEFAULT_CHANNEL == -1)
      assign default_src_channel = '0;
    else
      assign default_src_channel = 34'b1 << DEFAULT_CHANNEL;
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
    input  [106-1 : 0]    sink_data,
    input                       sink_startofpacket,
    input                       sink_endofpacket,
    output                      sink_ready,

    // -------------------
    // Command Source (Output)
    // -------------------
    output                          src_valid,
    output reg [106-1    : 0] src_data,
    output reg [34-1 : 0] src_channel,
    output                          src_startofpacket,
    output                          src_endofpacket,
    input                           src_ready
);

    // -------------------------------------------------------
    // Local parameters and variables
    // -------------------------------------------------------
    localparam PKT_ADDR_H = 62;
    localparam PKT_ADDR_L = 36;
    localparam PKT_DEST_ID_H = 95;
    localparam PKT_DEST_ID_L = 90;
    localparam ST_DATA_W = 106;
    localparam ST_CHANNEL_W = 34;
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
    localparam PAD11 = log2ceil(64'h40090d0 - 64'h40090c0);
    localparam PAD12 = log2ceil(64'h40090d8 - 64'h40090d0);
    localparam PAD13 = log2ceil(64'h40090e0 - 64'h40090d8);
    localparam PAD14 = log2ceil(64'h40090e8 - 64'h40090e0);
    localparam PAD15 = log2ceil(64'h40090ec - 64'h40090e8);
    localparam PAD16 = log2ceil(64'h40090f0 - 64'h40090ec);
    localparam PAD17 = log2ceil(64'h40090f4 - 64'h40090f0);
    localparam PAD18 = log2ceil(64'h40090f8 - 64'h40090f4);
    localparam PAD19 = log2ceil(64'h40090fc - 64'h40090f8);
    localparam PAD20 = log2ceil(64'h4009100 - 64'h40090fc);
    localparam PAD21 = log2ceil(64'h4009104 - 64'h4009100);
    localparam PAD22 = log2ceil(64'h4009108 - 64'h4009104);
    localparam PAD23 = log2ceil(64'h400910c - 64'h4009108);
    localparam PAD24 = log2ceil(64'h4009110 - 64'h400910c);
    localparam PAD25 = log2ceil(64'h4009114 - 64'h4009110);
    localparam PAD26 = log2ceil(64'h4009118 - 64'h4009114);
    localparam PAD27 = log2ceil(64'h400911c - 64'h4009118);
    localparam PAD28 = log2ceil(64'h4009120 - 64'h400911c);
    localparam PAD29 = log2ceil(64'h4009121 - 64'h4009120);
    localparam PAD30 = log2ceil(64'h4009122 - 64'h4009121);
    localparam PAD31 = log2ceil(64'h4009123 - 64'h4009122);
    localparam PAD32 = log2ceil(64'h4009124 - 64'h4009123);
    localparam PAD33 = log2ceil(64'h4009125 - 64'h4009124);
    // -------------------------------------------------------
    // Work out which address bits are significant based on the
    // address range of the slaves. If the required width is too
    // large or too small, we use the address field width instead.
    // -------------------------------------------------------
    localparam ADDR_RANGE = 64'h4009125;
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
    wire [34-1 : 0] default_src_channel;




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
            src_channel = 34'b0000000000000000000000000000000100;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 2;
        end

        // ( 0x4004000 .. 0x4008000 )
        if ( {address[RG:PAD1],{PAD1{1'b0}}} == 27'h4004000 ) begin
            src_channel = 34'b0000000000000000000000000000000010;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 1;
        end

        // ( 0x4008800 .. 0x4009000 )
        if ( {address[RG:PAD2],{PAD2{1'b0}}} == 27'h4008800 ) begin
            src_channel = 34'b0000000000000000000000000000000001;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 0;
        end

        // ( 0x4009000 .. 0x4009020 )
        if ( {address[RG:PAD3],{PAD3{1'b0}}} == 27'h4009000 ) begin
            src_channel = 34'b0000000000000000001000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 15;
        end

        // ( 0x4009020 .. 0x4009040 )
        if ( {address[RG:PAD4],{PAD4{1'b0}}} == 27'h4009020 ) begin
            src_channel = 34'b0000000000000000000000100000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 11;
        end

        // ( 0x4009040 .. 0x4009060 )
        if ( {address[RG:PAD5],{PAD5{1'b0}}} == 27'h4009040 ) begin
            src_channel = 34'b0000000000000000000000001000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 9;
        end

        // ( 0x4009060 .. 0x4009080 )
        if ( {address[RG:PAD6],{PAD6{1'b0}}} == 27'h4009060 ) begin
            src_channel = 34'b0000000000000000000000000000010000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 4;
        end

        // ( 0x4009080 .. 0x4009090 )
        if ( {address[RG:PAD7],{PAD7{1'b0}}} == 27'h4009080 ) begin
            src_channel = 34'b0000000100000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 26;
        end

        // ( 0x4009090 .. 0x40090a0 )
        if ( {address[RG:PAD8],{PAD8{1'b0}}} == 27'h4009090 ) begin
            src_channel = 34'b0000000000000000000100000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 14;
        end

        // ( 0x40090a0 .. 0x40090b0 )
        if ( {address[RG:PAD9],{PAD9{1'b0}}} == 27'h40090a0 ) begin
            src_channel = 34'b0000000000000000000000000100000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 8;
        end

        // ( 0x40090b0 .. 0x40090c0 )
        if ( {address[RG:PAD10],{PAD10{1'b0}}} == 27'h40090b0 ) begin
            src_channel = 34'b0000000000000000000000000010000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 7;
        end

        // ( 0x40090c0 .. 0x40090d0 )
        if ( {address[RG:PAD11],{PAD11{1'b0}}} == 27'h40090c0 ) begin
            src_channel = 34'b0000000000000000000000000000100000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 5;
        end

        // ( 0x40090d0 .. 0x40090d8 )
        if ( {address[RG:PAD12],{PAD12{1'b0}}} == 27'h40090d0 ) begin
            src_channel = 34'b0000000000000000000000010000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 10;
        end

        // ( 0x40090d8 .. 0x40090e0 )
        if ( {address[RG:PAD13],{PAD13{1'b0}}} == 27'h40090d8 ) begin
            src_channel = 34'b0000000000000000000000000001000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 6;
        end

        // ( 0x40090e0 .. 0x40090e8 )
        if ( {address[RG:PAD14],{PAD14{1'b0}}} == 27'h40090e0 ) begin
            src_channel = 34'b0000000000000000000000000000001000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 3;
        end

        // ( 0x40090e8 .. 0x40090ec )
        if ( {address[RG:PAD15],{PAD15{1'b0}}} == 27'h40090e8 ) begin
            src_channel = 34'b1000000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 33;
        end

        // ( 0x40090ec .. 0x40090f0 )
        if ( {address[RG:PAD16],{PAD16{1'b0}}} == 27'h40090ec ) begin
            src_channel = 34'b0010000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 31;
        end

        // ( 0x40090f0 .. 0x40090f4 )
        if ( {address[RG:PAD17],{PAD17{1'b0}}} == 27'h40090f0 ) begin
            src_channel = 34'b0001000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 30;
        end

        // ( 0x40090f4 .. 0x40090f8 )
        if ( {address[RG:PAD18],{PAD18{1'b0}}} == 27'h40090f4 ) begin
            src_channel = 34'b0000100000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 29;
        end

        // ( 0x40090f8 .. 0x40090fc )
        if ( {address[RG:PAD19],{PAD19{1'b0}}} == 27'h40090f8 ) begin
            src_channel = 34'b0000010000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 28;
        end

        // ( 0x40090fc .. 0x4009100 )
        if ( {address[RG:PAD20],{PAD20{1'b0}}} == 27'h40090fc ) begin
            src_channel = 34'b0000001000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 27;
        end

        // ( 0x4009100 .. 0x4009104 )
        if ( {address[RG:PAD21],{PAD21{1'b0}}} == 27'h4009100 ) begin
            src_channel = 34'b0000000001000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 24;
        end

        // ( 0x4009104 .. 0x4009108 )
        if ( {address[RG:PAD22],{PAD22{1'b0}}} == 27'h4009104 ) begin
            src_channel = 34'b0000000000100000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 23;
        end

        // ( 0x4009108 .. 0x400910c )
        if ( {address[RG:PAD23],{PAD23{1'b0}}} == 27'h4009108 ) begin
            src_channel = 34'b0000000000001000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 21;
        end

        // ( 0x400910c .. 0x4009110 )
        if ( {address[RG:PAD24],{PAD24{1'b0}}} == 27'h400910c ) begin
            src_channel = 34'b0000000000000100000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 20;
        end

        // ( 0x4009110 .. 0x4009114 )
        if ( {address[RG:PAD25],{PAD25{1'b0}}} == 27'h4009110 ) begin
            src_channel = 34'b0000000000000001000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 18;
        end

        // ( 0x4009114 .. 0x4009118 )
        if ( {address[RG:PAD26],{PAD26{1'b0}}} == 27'h4009114 ) begin
            src_channel = 34'b0000000000000000100000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 17;
        end

        // ( 0x4009118 .. 0x400911c )
        if ( {address[RG:PAD27],{PAD27{1'b0}}} == 27'h4009118 ) begin
            src_channel = 34'b0000000000000000000010000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 13;
        end

        // ( 0x400911c .. 0x4009120 )
        if ( {address[RG:PAD28],{PAD28{1'b0}}} == 27'h400911c ) begin
            src_channel = 34'b0000000000000000000001000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 12;
        end

        // ( 0x4009120 .. 0x4009121 )
        if ( {address[RG:PAD29],{PAD29{1'b0}}} == 27'h4009120 ) begin
            src_channel = 34'b0100000000000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 32;
        end

        // ( 0x4009121 .. 0x4009122 )
        if ( {address[RG:PAD30],{PAD30{1'b0}}} == 27'h4009121 ) begin
            src_channel = 34'b0000000010000000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 25;
        end

        // ( 0x4009122 .. 0x4009123 )
        if ( {address[RG:PAD31],{PAD31{1'b0}}} == 27'h4009122 ) begin
            src_channel = 34'b0000000000010000000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 22;
        end

        // ( 0x4009123 .. 0x4009124 )
        if ( {address[RG:PAD32],{PAD32{1'b0}}} == 27'h4009123 ) begin
            src_channel = 34'b0000000000000010000000000000000000;
            src_data[PKT_DEST_ID_H:PKT_DEST_ID_L] = 19;
        end

        // ( 0x4009124 .. 0x4009125 )
        if ( {address[RG:PAD33],{PAD33{1'b0}}} == 27'h4009124 ) begin
            src_channel = 34'b0000000000000000010000000000000000;
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


