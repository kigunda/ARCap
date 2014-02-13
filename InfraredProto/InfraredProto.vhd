-- Nancy Minderman
-- nancy.minderman@ualberta.ca
-- This file makes extensive use of Altera template structures.
-- This file is the top-level file for lab 1 winter 2014 for version 12.1sp1 on Windows 7


-- A library clause declares a name as a library.  It 
-- does not create the library; it simply forward declares 
-- it. 
library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

	-- SIGNED and UNSIGNED types, and relevant functions
	use ieee.numeric_std.all;

	-- Basic sequential functions and concurrent procedures
	use ieee.VITAL_Primitives.all;
	
	use work.DE0_CONSTANTS.all;
	
	entity InfraredProto is
	
	port
	(
		-- 50 MHz Clock
		CLOCK_50	: in  std_logic;
	
		-- Input ports
		KEY		: in  std_logic_vector (1 downto 0);		-- pushbuttons
		GPIO_0	: inout std_logic_vector (0 downto 0);		-- expansion header
		LED 		: out DE0_LED;										-- LEDs

		-- SDRAM on board
		DRAM_DQ		:	inout DE0_SDRAM_DATA_BUS;
		DRAM_ADDR	:	out	DE0_SDRAM_ADDR_BUS;
		DRAM_DQM		:	out	DE0_SDRAM_DATA_MASK_BUS;
		DRAM_CLK		:	out	std_logic;
		DRAM_CKE		:	out	std_logic;
		DRAM_WE_N	: 	out 	std_logic;
		DRAM_CAS_N	:	out	std_logic;
		DRAM_RAS_N	: 	out	std_logic;
		DRAM_CS_N	:	out	std_logic;
		DRAM_BA_0	: 	out	std_logic;
		DRAM_BA_1	:	out	std_logic
		
	);
end InfraredProto;


architecture structure of InfraredProto is

	-- Declarations (optional)
	
	 component project1 is
        port (
            clk_clk                                 	: in	  std_logic				:= 'X';             	-- clk
            reset_reset_n                           	: in	  std_logic				:= 'X';             	-- reset_n
				sdram_0_wire_dq                         	: inout DE0_SDRAM_DATA_BUS := (others => 'X'); 	-- dq
            sdram_0_wire_addr                       	: out   DE0_SDRAM_ADDR_BUS;                    	-- addr
            sdram_0_wire_dqm                        	: out   DE0_SDRAM_DATA_MASK_BUS;						-- dqm
				sdram_0_wire_cke                        	: out   std_logic;                              -- cke
				sdram_0_wire_we_n                       	: out   std_logic;                              -- we_n
            sdram_0_wire_cas_n                      	: out   std_logic;                             	-- cas_n
            sdram_0_wire_ras_n                      	: out   std_logic;                             	-- ras_n
            sdram_0_wire_cs_n                       	: out   std_logic;                              -- cs_n
				sdram_0_wire_ba                         	: out   std_logic_vector(1 downto 0);           -- ba
            altpll_0_c0_clk                         	: out   std_logic;										-- sdram clk
				leds_external_connection_export   			: out   DE0_LED;											-- export LEDs
				push_button_external_connection_export		: in	  std_logic;										-- export pushbutton
				ir_emitter_external_connection_export		: out	  std_logic											-- export ir emitter signal
        );
    end component project1;

	 -- These signals are for matching the provided IP core to
	 -- The specific SDRAM chip in our system	 
	 signal BA	: std_logic_vector (1 downto 0);
	 
begin
	
	DRAM_BA_1 <= BA(1);
	DRAM_BA_0 <= BA(0);
	
	-- Component Instantiation Statement (optional)
	
	  u0 : component project1
        port map (
            clk_clk                                 	=> CLOCK_50,                                
            reset_reset_n                           	=> KEY(0),
				sdram_0_wire_dq                         	=> DRAM_DQ,
            sdram_0_wire_addr                       	=> DRAM_ADDR,
            sdram_0_wire_dqm                        	=> DRAM_DQM,                    
            sdram_0_wire_cke                        	=> DRAM_CKE,                     
            sdram_0_wire_we_n                       	=> DRAM_WE_N,                
            sdram_0_wire_cas_n                      	=> DRAM_CAS_N,                
            sdram_0_wire_ras_n                      	=> DRAM_RAS_N,                        
            sdram_0_wire_cs_n                       	=> DRAM_CS_N,  
            sdram_0_wire_ba                         	=> BA,         
            altpll_0_c0_clk                         	=> DRAM_CLK,		                                               		
            leds_external_connection_export   			=> LED,
				push_button_external_connection_export		=> KEY(1),
				ir_emitter_external_connection_export		=> GPIO_0(0)
        );

end structure;

library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

package DE0_CONSTANTS is
	
	type DE0_SDRAM_ADDR_BUS 		is array(12 downto 0) of std_logic;
	type DE0_SDRAM_DATA_BUS 		is array(15 downto 0) of std_logic;
	type DE0_SDRAM_DATA_MASK_BUS	is array( 1 downto 0) of std_logic;
	
	type DE0_LED						is array(7 downto 0)  of std_logic;
	
end DE0_CONSTANTS;