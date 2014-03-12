--*****************************************************************************
--*  Copyright (c) 2012 by Michael Fischer. All rights reserved.
--*
--*  Redistribution and use in source and binary forms, with or without 
--*  modification, are permitted provided that the following conditions 
--*  are met:
--*  
--*  1. Redistributions of source code must retain the above copyright 
--*     notice, this list of conditions and the following disclaimer.
--*  2. Redistributions in binary form must reproduce the above copyright
--*     notice, this list of conditions and the following disclaimer in the 
--*     documentation and/or other materials provided with the distribution.
--*  3. Neither the name of the author nor the names of its contributors may 
--*     be used to endorse or promote products derived from this software 
--*     without specific prior written permission.
--*
--*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
--*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
--*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
--*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
--*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
--*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
--*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
--*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
--*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
--*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
--*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
--*  SUCH DAMAGE.
--*
--*****************************************************************************
--*  History:
--*
--*  14.07.2012  mifi  First Version, tested with an Altera DE1 and
--*                    Terasic DE0-Nano board. Here an Olimex DM9000E-H
--*                    header board was used.
--*  01.08.2012  mifi  Final version, which is used for the tutorial.
--*  15.08.2012  mifi  Added RESET port. 
--*****************************************************************************


--*****************************************************************************
--*  DEFINE: Library                                                          *
--*****************************************************************************

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;


--*****************************************************************************
--*  DEFINE: Entity                                                           *
--*****************************************************************************

entity dm9000e_if is
	port (
          -- Avalon Slave bus
		    clk          : in    std_logic := '0';
		    reset        : in    std_logic := '0';
		    address      : in    std_logic_vector(1 downto 0) := (others => '0');
          chipselect_n : in    std_logic := '1';
          write_n      : in    std_logic := '1';
          writedata    : in    std_logic_vector(15 downto 0) := (others => '0');
          read_n       : in    std_logic := '1';
          readdata     : out   std_logic_vector(15 downto 0);
          irq          : out   std_logic;
      
          -- External bus
		    ENET_CMD     : out   std_logic;
		    ENET_DATA    : inout std_logic_vector(15 downto 0) := (others => '0');
		    ENET_WE_N    : out   std_logic;
		    ENET_OE_N    : out   std_logic;
		    ENET_CE_N    : out   std_logic;
          ENET_RESET_N : out   std_logic;
		    ENET_INT     : in    std_logic := '0'
	     );
end entity dm9000e_if;

--*****************************************************************************
--*  DEFINE: Architecture                                                     *
--*****************************************************************************

architecture syn of dm9000e_if is

   --
   -- Define all constants here
   --
   constant ADDR_COMMAND : integer := 0;
   constant ADDR_DATA    : integer := 1;
   constant ADDR_RESET   : integer := 2;


   --
   -- Define all components which are included here
   --

   --
   -- Define all local signals (like static data) here
   --
   signal reset_reg : std_logic := '0';

begin

   process(clk)
   begin
      if rising_edge(clk) then
         if (write_n = '0') then
            if (ADDR_RESET = unsigned(address)) then  -- ADDR_RESET
               reset_reg <= writedata(0);
            end if;             
         end if;
      end if; 
   end process;


   ENET_CMD     <= address(0);
   ENET_WE_N    <= write_n;
   ENET_OE_N    <= read_n;
   ENET_CE_N    <= chipselect_n when (address(1) ='0') else '1';
   ENET_RESET_N <= reset_reg;
   irq          <= ENET_INT;

   ENET_DATA    <= writedata when (write_n = '0') else (others => 'Z');   
   readdata     <= ENET_DATA;

end architecture syn;

-- *** EOF ***
