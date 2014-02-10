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
		-- Input ports and 50 MHz Clock
		KEY		: in  std_logic_vector (0 downto 0);
		CLOCK_50	: in  std_logic;

		-- SRAM on board
		SRAM_ADDR	:	out	DE0_SRAM_ADDR_BUS;
		SRAM_DQ		:	inout DE0_SRAM_DATA_BUS;
		SRAM_WE_N	:	out	std_logic;
		SRAM_OE_N	:	out	std_logic;
		SRAM_UB_N	:	out 	std_logic;
		SRAM_LB_N	:	out	std_logic;
		SRAM_CE_N	:	out	std_logic;
		
		-- GPIO for infrared signals
		GPIO_0 	: out std_logic_vector (0 downto 0)
		
	);
end InfraredProto;


architecture structure of InfraredProto is

	-- Declarations (optional)
	
	 component niosII_system is
        port (
            clk_clk                                 			: in		std_logic			:= 'X';             	-- clk
            reset_reset_n                           			: in		std_logic			:= 'X';             	-- reset_n
            sram_0_external_interface_DQ            			: inout	DE0_SRAM_DATA_BUS := (others => 'X');	-- DQ
            sram_0_external_interface_ADDR          			: out		DE0_SRAM_ADDR_BUS;							-- ADDR
            sram_0_external_interface_LB_N          			: out		std_logic;										-- LB_N
            sram_0_external_interface_UB_N          			: out		std_logic;										-- UB_N
            sram_0_external_interface_CE_N          			: out		std_logic;										-- CE_N
            sram_0_external_interface_OE_N          			: out		std_logic;										-- OE_N
            sram_0_external_interface_WE_N          			: out		std_logic;										-- WE_N
				infrared_controller_0_conduit_end_0_export		: in		std_logic										-- export infrared emitter signal
        );
    end component niosII_system;

begin
	
	-- Component Instantiation Statement (optional)
	
	  u0 : component niosII_system
        port map (
            clk_clk                                 			=> CLOCK_50,                                
            reset_reset_n                           			=> KEY(0),                                                
            green_leds_external_connection_export   			=> LEDG,       
            sram_0_external_interface_DQ            			=> SRAM_DQ,           
            sram_0_external_interface_ADDR          			=> SRAM_ADDR,          
            sram_0_external_interface_LB_N          			=> SRAM_LB_N,         
            sram_0_external_interface_UB_N          			=> SRAM_UB_N,          
            sram_0_external_interface_CE_N          			=> SRAM_CE_N,         
            sram_0_external_interface_OE_N          			=> SRAM_OE_N,         
            sram_0_external_interface_WE_N          			=> SRAM_WE_N,
				infrared_controller_0_conduit_end_0_export		=> GPIO_0(0)
        );

end structure;

library ieee;

-- Commonly imported packages:

	-- STD_LOGIC and STD_LOGIC_VECTOR types, and relevant functions
	use ieee.std_logic_1164.all;

package DE0_CONSTANTS is
	
	type DE0_SDRAM_ADDR_BUS is array(11 downto 0) of std_logic;
	type DE0_SDRAM_DATA_BUS is array(15 downto 0) of std_logic;
	
	type DE0_LED_GREEN		is array(7 downto 0)  of std_logic;
	
	type DE0_SRAM_ADDR_BUS	is array(17 downto 0) of std_logic;
	type DE0_SRAM_DATA_BUS  is array(15 downto 0) of std_logic;
	
end DE0_CONSTANTS;