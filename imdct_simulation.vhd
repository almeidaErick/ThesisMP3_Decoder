----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.10.2017 14:20:38
-- Design Name: 
-- Module Name: imdct_simulation - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity imdct_simulation is
--  Port ( );
end imdct_simulation;

architecture Behavioral of imdct_simulation is
    component imdct_process port
    (
        clk : in std_logic;
        input_val : in std_logic_vector(31 downto 0); -- input value that is going to be stored into the input block ram
        output_val : out Integer; -- value from output RAM
        address_in : in std_logic_vector(7 downto 0); -- address to control what register to write to input block RAM
        address_out : in std_logic_vector(7 downto 0); -- address to control what register to read from output block RAM
        block_type : in std_logic_vector(1 downto 0); -- type of block (long or short block) for IMCDT block to know how many inputs should read
        done_writing_input : in std_logic; -- send from PS to PL to know when start reading inputo block RAM
        mode_input_output : in std_logic; -- set writing or reading mode for input block RAM (only used in input RAM)
        done_writing_output : out Integer -- from PL to PS to start reading the output block RAM
    );
    end component;
    
    signal clk : std_logic := '0';
    signal input_val : std_logic_vector(31 downto 0) := X"00000000";
    signal output_val : Integer; -- no value here
    signal address_in : std_logic_vector(7 downto 0) := X"00"; 
    signal address_out : std_logic_vector(7 downto 0) := X"00"; 
    signal block_type : std_logic_vector(1 downto 0) := b"01"; 
    signal done_writing_input : std_logic := '0'; 
    signal mode_input_output : std_logic := '0'; 
    signal done_writing_output : Integer; -- no value here
    
begin

    clk <= not clk after 100ns;
    --done_writing_input <= '1' after 200ns;
    --done_writing_input <= '0' after 250us;
    
    uut: imdct_process PORT MAP
    (
    clk => clk,
    input_val => input_val,
    output_val => output_val,
    address_in => address_in, 
    address_out => address_out,
    block_type => block_type,
    done_writing_input => done_writing_input,
    mode_input_output => mode_input_output,
    done_writing_output => done_writing_output       
    );
    
    stim_proc: process
    begin
        wait for 200ns;
        done_writing_input <= '1';
        wait for 200us; --for short
        --wait for 450us;
        done_writing_input <= '0';
    end process;


end Behavioral;
