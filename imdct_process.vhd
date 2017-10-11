----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 29.09.2017 16:19:28
-- Design Name: 
-- Module Name: imdct_block - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description:
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity imdct_process is
--  Port ( );
    port(
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
end imdct_process;

architecture IMP of imdct_process is

-- add component below this line

component COS_RAM Port
    (
        Address : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER
        --table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end component;


component COS_TAB1_ROM Port(
        row : in STD_LOGIC_VECTOR(7 downto 0);
        col : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER
        --table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end component;


component WIN_ROM Port(
        row : in STD_LOGIC_VECTOR(7 downto 0);
        col : in STD_LOGIC_VECTOR(7 downto 0);
        clock : in STD_LOGIC;
        output_data : out INTEGER
        --table_select : in STD_LOGIC_VECTOR(1 downto 0)
    );
end component;


component INPUT_RAM is Port (
        clock : in std_logic;
        data_out : out Integer; -- from block RAM to PL
        data_in : in Integer; -- from PS to block RAM
        address_in : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to be saved
        address_out : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to read
        mode : in std_logic
    );
end component;


component OUTPUT_RAM is Port (
        clock : in std_logic;
        data_out : out Integer; -- from block RAM to PS
        data_out_read : out Integer; -- from block RAM to PL
        data_in : in Integer; -- from PL to block RAM
        address_in : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to be saved
        address_out : in std_logic_vector(7 downto 0); -- control the index where the specific value is going to read
        mode : in std_logic;
        reset : in std_logic
    );
end component;

-- add components above this line

-- SIGNALS USED FOR INPUT SIGNALS FROM FILE (imdct_process.vhd) START HERE --------------------------------------------------------------------------------------------------------
-- add signals below
signal ps_input_integer : Integer; -- goes from input (PS) to input block ram
signal ps_output_integer : Integer; -- goes from output block ram to PS.
signal pl_input_integer : Integer := 0; -- goes from (PL) to output block ram, initialized as 0..!!!


signal pl_input_integer_short : Integer := 0;  --input signal that goes from PL to output block rame (short block)
signal pl_input_integer_long : Integer := 0; --input signal that goes from PL to output block rame (long block)


signal pl_output_integer : Integer; -- goes from input block ram to PL.
signal pl_outputBlock_integer : Integer := 0; -- goes from output block ram to PL.
signal clock_control : std_logic; -- clock that will control every other block 
signal full_reset : std_logic := '0'; --clear output block ram..!!!

-- signals to control block RAM (input or output) start here
-- PS 
signal ps_address_in : std_logic_vector(7 downto 0); -- write to input block ram
signal ps_address_out: std_logic_vector(7 downto 0); -- read from output block ram

--PL
signal pl_address_in : std_logic_vector(7 downto 0) := X"00"; -- write to output block ram (maped directly to block ram)

signal pl_address_in_long : std_logic_vector(7 downto 0) := X"00"; -- write to output block ram (selected between long and whort signal to send to pl_address_in)
signal pl_address_in_short : std_logic_vector(7 downto 0) := X"00"; -- write to output block ram (selected between long and whort signal to send to pl_address_in)

signal pl_address_out: std_logic_vector(7 downto 0); -- read from input block ram

signal pl_address_out_short: std_logic_vector(7 downto 0);
signal pl_address_out_long: std_logic_vector(7 downto 0);
-- signals to control block RAM (input or output) end here

signal block_type_frame : std_logic_vector(1 downto 0); -- come from input from PS to PL
signal done_input_block : std_logic; -- com from PS to PL indicates when the input block ram is full

-- Signals to control state from input block ram and output block ram start here
--PS
signal ps_mode_control_block : std_logic; -- come from PS to PL to control input block ram
--PL
signal pl_mode_control_block : std_logic; -- come from PL file (imdct_process.vhd) to output block ram 
-- Signals to control state from input block ram and output block ram end here

signal done_output_block : Integer; -- com from PL to PS indicates when the output block ram is full
-- SIGNALS USED FOR INPUT SIGNALS FROM FILE (imdct_process.vhd) END HERE --------------------------------------------------------------------------------------------------------


-- SIGNALS TO CONTROL LOOK UP TABLES START HERE ---------------------------------------------------------------------------------------------------------------------------------
signal cos_address_table : std_logic_vector(7 downto 0); -- control address for COS_RAM look up table
--signal table_selection : std_logic_vector(1 downto 0); -- control from which table are we going to read from 

--signal table_selection_short : std_logic_vector(1 downto 0); 
--signal table_selection_long : std_logic_vector(1 downto 0); 


signal cos_block_out : integer; -- send COS_ROM output
signal cos_tab_out : integer; -- send COS_TAB1_ROM output
signal win_block_out : integer; -- send WIN_ROM output;


signal win_row : STD_LOGIC_VECTOR(7 downto 0); -- row address for WIN_ROM table
signal win_col : STD_LOGIC_VECTOR(7 downto 0); -- column address for WIN_ROM table


signal win_row_short : STD_LOGIC_VECTOR(7 downto 0);
signal win_row_long : STD_LOGIC_VECTOR(7 downto 0);

signal win_col_short : STD_LOGIC_VECTOR(7 downto 0);
signal win_col_long : STD_LOGIC_VECTOR(7 downto 0);


signal cos_tab_row : STD_LOGIC_VECTOR(7 downto 0); -- row address for COS_TAB1_ROW table
signal cos_tab_col : STD_LOGIC_VECTOR(7 downto 0); -- column address for COS_TAB1_ROW table
-- SIGNALS TO CONTROL LOOK UP TABLES END HERE ---------------------------------------------------------------------------------------------------------------------------------

-- SIGNALS TO CONTROL FSM FOR PROCESSING IMDCT FUNCTION START HERE ------------------------------------------------------------------------------------------------------------
TYPE writing_setup IS (write_input_buffer, wait_to_input, write_output_buffer, wait_to_output);
SIGNAL writing_control: writing_setup := write_input_buffer;

-- signals to start short or long blocks
signal short_block_start : std_logic := '0';
signal long_block_start : std_logic := '0';

TYPE short_block IS (S0, S1, S2, S3, S4, S5, S6, S7, S8);
SIGNAL short_block_control: short_block := S0; --check notes to see FSM diagram
signal start_saving_short : std_logic := '0'; -- start writing elements to output block
signal section_output_done : std_logic := '0'; -- check for short block (in process that writes output short block), 
-- section_output_done is controlled in save_out_ram proces..!!!!!!!

TYPE save_part_output IS (S0, S1, S2, Sx);
SIGNAL save_out_ram : save_part_output := S0;
signal get_index_i : integer := 0;



TYPE long_block IS (S0, S1, S2, S3, S4, S5, S6);
SIGNAL long_block_control: long_block := S0;

TYPE send_output_signal IS (S0, S1, S2);
SIGNAL send_output_control: send_output_signal:= S0;

signal done_writing_long : std_logic := '0';
signal done_writing_short : std_logic := '0';

--temp array,,!!!
type temporal_array is array (0 to 11) of integer;
signal tmp : temporal_array := (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); -- access to elements of array like ... tmp(1) <= 1234; used in short block process
--signal sum_tot : integer := 0; -- used to pass from process contro_short_block to write_output process

-- SIGNALS TO CONTROL FSM FOR PROCESSING IMDCT FUNCTION END HERE ------------------------------------------------------------------------------------------------------------
-- add signals above

signal testing : integer;

begin
    clock_control <= clk;
    ps_mode_control_block <= mode_input_output;
    ps_input_integer <= (to_integer(signed(input_val)));
    ps_address_in <= address_in;
    ps_address_out <= address_out;
    output_val <= ps_output_integer;
    block_type_frame <= block_type;
    
    done_writing_output <= done_output_block; 
    done_input_block <= done_writing_input;
    
    
    pl_address_in <= pl_address_in_short when short_block_start = '1' else pl_address_in_long; -- this statement is working
    pl_address_out <= pl_address_out_short when short_block_start = '1' else pl_address_out_long; -- this statement is  working
    pl_input_integer <= pl_input_integer_short when short_block_start = '1' else pl_input_integer_long; -- this statement is  working
    --table_selection <= table_selection_short when short_block_start = '1' else table_selection_long; -- this statement is now working
    
    win_row <= win_row_short when short_block_start = '1' else win_row_long;
    win_col <= win_col_short when short_block_start = '1' else win_col_long;
    
     
    
    -- map COS_RAM to get values from look up table
    g1: COS_RAM PORT MAP
    (
        Address => cos_address_table, -------------------------------------- control from PL...!!!
        clock => clock_control,
        output_data => cos_block_out -------------------------------------- control from PL...!!!
        --table_select => table_selection -------------------------------------- control from PL...!!!    
    );
    
    -- map COS_TAB1_ROM to get values from look up table
    g2: COS_TAB1_ROM PORT MAP
    (
        row => cos_tab_row, -------------------------------------- control from PL...!!!
        col => cos_tab_col, -------------------------------------- control from PL...!!!
        clock => clock_control,
        output_data => cos_tab_out -------------------------------------- control from PL...!!!
        --table_select => table_selection -------------------------------------- control from PL...!!!
    );   
    
    -- map WIN_ROM to get values from look up table
    g3: WIN_ROM PORT MAP
    (
        row => win_row, -------------------------------------- control from PL...!!!
        col => win_col, -------------------------------------- control from PL...!!!
        clock => clock_control,
        output_data => win_block_out -------------------------------------- control from PL...!!!
        --table_select => table_selection -------------------------------------- control from PL...!!!
    );
    
    -- map INPUT_RAM to populate input block ram from PS and read it back from PL
    g4: INPUT_RAM PORT MAP
    (
        clock => clock_control,
        data_out => pl_output_integer, -- (input ram) to PL --------------------------------------- used in PL..!!!
        data_in => ps_input_integer, --PS to (input ram)
        address_in => ps_address_in, -- write from PS to input ram
        address_out => pl_address_out, -- read from PL to input ram ----------------------------- control from PL..!!!!!
        mode => ps_mode_control_block -- input ram mode controled by PS
    );
    
    -- map OUTPUT_RAM to populate output block ram from PL and read it back from PS
    g5: OUTPUT_RAM PORT MAP
    (
        clock => clock_control,
        data_out => ps_output_integer, -- (output ram) to PS
        data_out_read => pl_outputBlock_integer,
        data_in => pl_input_integer,  -- PL to (output ram) ------------------------------------------------------ control from PL
        address_in => pl_address_in, -- write from PL to output ram -------------------------------------------- control from PL
        address_out => ps_address_out, -- read from PS to (output ram) PL
        mode => pl_mode_control_block, -- output ram mode controled by PL ---------------------------------------------- control from PL
        reset => full_reset
    );

--    process(clk)
--    begin
--        if (rising_edge(clk)) then
--            output_val <= (to_integer(signed(input_val)) * to_integer(signed(input_val))* to_integer(signed(input_val)));
--        end if;
--    end process;
    
    -- Start process tocontrol the signal for short and long blocks, also check here when the signal for finishing writing the output and input block ram changes..!!!!
    process(clk)
    begin
        if(rising_edge(clk)) then
              case writing_control is 
                when write_input_buffer =>
                    if(done_input_block = '1') then
                        if(block_type_frame = b"10") then
                            short_block_start <= '1';
                        else
                            long_block_start <= '1';
                        end if;
                        pl_mode_control_block <= '1'; -- set block ram output as write mode..!!!
                        writing_control <= wait_to_input;
                    end if;
                
                when wait_to_input =>
   
--                    case short_block_start is
--                        when '1' => pl_address_in <= pl_address_in_short;
--                        when others => pl_address_in <= pl_address_in_long;
--                    end case;
--                    if(done_input_block = '0') then
--                        writing_control <= write_output_buffer;
--                    end if;
                    writing_control <= write_output_buffer;
--                    short_block_start <= '0';
--                    long_block_start <= '0';
                
                when write_output_buffer =>
                    -- done_writing_output just gonna be high for one clock cycle...!!!!! check this..!!
                    if(done_output_block = 1) then
                        writing_control <= wait_to_output;
                        short_block_start <= '0';
                        long_block_start <= '0';
                        pl_mode_control_block <= '0'; -- set block ram output to read mode..!!!
                    end if;
                    
                when wait_to_output =>
                    -- Here wait until write_input signal has changed from 1 to 0
                    -- Here done_writing_output i 0..!!!! check in simulation..!!
                    if(done_input_block = '0') then
                        writing_control <= write_input_buffer;
                    end if;
                    
              end case;
        end if;
    end process;
    -- End process tocontrol the signal for short and long blocks, also check here when the signal for finishing writing the output and input block ram changes..!!!!

    -- Start process to control short block 
    process(clk)
    variable i : integer := 0;
    variable m : integer := 0;
    --variable N : integer;
    variable p : integer := 0;
    variable k : integer := 0;
    variable hold : integer := 0;
    variable sum : integer := 0;

    begin
        if(rising_edge(clk)) then
            case short_block_control is
                when S0 =>
                    if(short_block_start = '1') then
                        short_block_control <= S1;
                    else
                        i := 0;
                        get_index_i <= i;
                        m := 0;
                        p := 0;
                        hold := 0;
                        k := 0;
                        sum := 0;
                        --done_output_block <= 0;
                        done_writing_short <= '0';
                    end if;
                when S1 =>
                    if(i < 3) then
                        short_block_control <= S2;
                        p := 0; -- restart inner loop..!!! (p)
                    else
                        short_block_control <= S0;
                        --done_output_block <= 1;
                        done_writing_short <= '1';
                        --add here output signal
                    end if;
                when S2 =>
                    if(p < 12) then
                        short_block_control <= S3;
                        m := 0; -- restart inner loop..!!! (m)
                        sum := 0; -- restart sum...!!!
                    else
                        start_saving_short <= '1';
                        short_block_control <= S8;
                    end if;
                when S3 =>
                    if(m < 6) then
                        --k := (m) + (m sll 1);
                        --k := (m) + shift_left(unsigned(m), 1);
                        k := m + (2*m);
                        hold := i + k; -- to make sure we tae the correct index of the array (used for input block ram)
                        short_block_control <= S4;
                        
                        -- START add indexes for control block called COS_TAB_1
                        cos_tab_row <= std_logic_vector(to_unsigned(p, cos_tab_row'length));
                        cos_tab_col <= std_logic_vector(to_unsigned(m, cos_tab_col'length));
                        --END add indexes for control block called COS_TAB_1
                       
                        
                        -- START add index for reading from input block ram
                        pl_address_out_short <= std_logic_vector(to_unsigned(hold, pl_address_out_short'length));
                        --pl_address_out <= std_logic_vector(to_unsigned(hold, pl_address_out'length));
                        -- END add index for reading from input block ram
                    else 
                        short_block_control <= S6;
                        
                        -- START getting index for table WIN_ROM here
                        win_row_short <= b"000000"&block_type_frame;
                        win_col_short <= std_logic_vector(to_unsigned(p, win_col'length));
                        -- END getting index for table WIN_ROM here
                    end if;
                when S4 =>
                    -- add sum here..!!!!!! sum += in[hold] * (cos_tab_1[p][m])
                    --sum := sum + (pl_output_integer * cos_tab_out);
                    testing <= cos_tab_out * 2;
                    short_block_control <= S5;
                when S5 =>
                    sum := sum + (pl_output_integer * cos_tab_out);
                    m := m + 1;
                    short_block_control <= S3;
                when S6 =>
                
                    short_block_control <= S7;
                when S7 =>
                    -- START calculate tmp[p] here..!!!!
                    tmp(p) <= sum * win_block_out;
                    p := p + 1;
                    short_block_control <= S2;
                when S8 =>
                    start_saving_short <= '0';
                    if(section_output_done = '1') then
                        short_block_control <= S1;
                        i := i + 1;
                        get_index_i <= i;
                    end if;
            end case;
        end if;
    end process;
    -- End process to control short block
    
    -- Start process of saving samples to output ram..!!
    process(clk)
    variable p : integer := 0;
    variable k : integer := 0;
    variable hold : integer := 0;
    variable previous_out_ram : integer := 0;
    begin
        if(rising_edge(clk)) then
            case save_out_ram is
                when S0 =>
                    if(start_saving_short = '1') then
                        save_out_ram <= S1;
                        k := get_index_i*6;
                        hold := k + p + 6;
                    else
                        p := 0;
                        -- set block ram to read mode
                        --pl_mode_control_block <= '0';
                    end if;
                    section_output_done <= '0'; -- outputs are not ready ..!!
             
                when S1 =>
                    if(p < 12) then
                        -- write here to output block ram..!!!!!! use hold index..!!!
                        -- pl_input_integer <= pl_input_integer + tmp(p); --ORIGINAL>>!! 
                        -- pl_input_integer <= tmp(p);
                        
                        -- START sending data to output block ram
                        -- set address for output block ram
                        pl_address_in_short <= std_logic_vector(to_unsigned(hold, pl_address_in_short'length));
                        -- set input for output block
                        --pl_input_integer <= pl_outputBlock_integer + tmp(p); -- adding previously saved number on output ram and tmp(p) to create a new entry at the specified index of the block ram
                        --pl_input_integer <= tmp(p);
                        --previous_out_ram := pl_outputBlock_integer + tmp(p);
                        -- set block ram to write mode
                        --pl_mode_control_block <= '1';
                        -- END sending data to output block ram
                        
                        save_out_ram <= Sx;
                    else
                        section_output_done <= '1';
                        save_out_ram <= S0;
                        
                        -- set block ram to read mode
                        --pl_mode_control_block <= '0';
                    end if;
                when Sx =>
                    save_out_ram <= S2;
                when S2 =>
                    --pl_input_integer <= pl_outputBlock_integer + tmp(p); -- adding previously saved number on output ram and tmp(p) to create a new entry at the specified index of the block ram
                    -- Here just change variables..!! 
                    -- Give one more clock cycle to get previous value saved..!!
                    --pl_input_integer <= previous_out_ram;
                    --pl_input_integer <= pl_outputBlock_integer + tmp(p); -- adding previously saved number on output ram and tmp(p) to create a new entry at the specified index of the block ram
                    pl_input_integer_short <= pl_outputBlock_integer + tmp(p); -- adding previously saved number on output ram and tmp(p) to create a new entry at the specified index of the block ram
                    p := p + 1;
                    save_out_ram <= S1;
                    hold := k + p + 6;
                    --pl_mode_control_block <= '1';
                    -- set address for output block ram
                    --pl_address_in <= std_logic_vector(to_unsigned(hold, pl_address_in'length));
                    
                    --pl_input_integer <= tmp(p); overflow here, do not put this variable here..!!!
            end case;
        end if;
    end process;
    -- End process of saving samples to output ram..!!
    
    
    -- Start process to control long block 
    process(clk)
    variable k : integer := 0;
    variable p : integer := 0;
    variable m : integer := 0;
    variable sum : integer := 0;
    begin
        if(rising_edge(clk)) then
            case long_block_control is
                when S0 =>
                    if(long_block_start = '1') then
                        long_block_control <= S1;
                    else
                        m := 0;
                        p := 0;
                        k := 0;
                        --done_output_block <= 0;
                        sum := 0; -- restart sum...!!!
                        done_writing_long <= '0';
                    end if;
                when S1 =>
                    if(p < 36) then
                        long_block_control <= S2;
                        m := 0; -- start inner loop again..!!!
                        sum := 0; -- restart sum...!!!
                        -- set address for output block ram
                        pl_address_in_long <= std_logic_vector(to_unsigned(p, pl_address_in_short'length));
                    else
                        long_block_control <= S0;
                        -- add here output signal..!!!!!
                        --done_output_block <= 1; -- check this..!!! if signal is enough to keep it up for one clock cycle
                        done_writing_long <= '1';
                    end if;
                when S2 =>
                    if(m < 18) then
                        long_block_control <= S3;
                        -- calculate here k..!!!
                        -- int k = ((p*m)<<2) + (p<<1) + (m<<5)+(m<<2)+(m<<1)+19;
                        k := ((p * m) * 4) + (p * 2) + (m * 32) + (m * 4) + (m * 2) + 19;
                        
                        -- START add index for reading from input block ram
                        pl_address_out_long <= std_logic_vector(to_unsigned(m, pl_address_out_long'length));
                        -- END add index for reading from input block ram
                        
                        
                    else 
                        long_block_control <= S5;
                        -- send output values here..!!!!
                        
                        -- START getting index for table WIN_ROM here
                        win_row_long <= b"000000"&block_type_frame;
                        win_col_long <= std_logic_vector(to_unsigned(p, win_col'length));
                        -- END getting index for table WIN_ROM here
                        
                    end if; 
                when S3 =>
                    if(k >= 144) then
                        k := k - 144;
                    else 
                        long_block_control <= S4;
                        cos_address_table <= std_logic_vector(to_unsigned(k, cos_address_table'length)); -- give address for reading values from COS_RAM block ram
                    end if;
                    
                when S4 =>
                    sum := sum + (pl_output_integer * cos_block_out);
                    m := m + 1;
                    long_block_control <= S2;
               
                when S5 =>
                    long_block_control <= S6;
                when S6 =>
                    pl_input_integer_long <= sum * win_block_out; -- adding result to output block ram
                    p := p + 1;
                    long_block_control <= S1;
                
            end case;
        end if;
    end process;
    -- End process to control long block
    
    -- Start control output signal (this was added since a signal can only be modified from a single process ..!! )
    process(clk)
    begin
        if(rising_edge(clk)) then
            case send_output_control is
                when S0 =>
                    if ((done_writing_long = '1') or (done_writing_short = '1')) then
                        send_output_control <= S1;
                        done_output_block <= 1;
                    end if;
                when S1 =>
                    if(done_input_block = '0') then -- add here to wait until done_input is 0, this step is used in 2 processes.
                        done_output_block <= 0;
                        send_output_control <= S2;
                        full_reset <= '1';
                    end if;
                when S2 =>
                    full_reset <= '0';
                    send_output_control <= S0;
            end case;
        end if;
    end process;
    -- End control output signal..!
end IMP;
