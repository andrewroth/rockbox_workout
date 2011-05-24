#! /usr/bin/ruby
name = "workout"

# start a screen if it exists
def start_with_check(name)
  unless %x[screen -ls] =~ /\d+\.#{name}/
    puts "Not found, starting."
    system "screen -d -m -S #{name} &"
    sleep 1
  end
end

#start_with_check name
#system screen -S #{name} -X kill"
start_with_check name
#system "screen -S #{name} -p 0 -X exec cat" # run something in case nothing is running so kill doesn't close screen
#sleep 1
#system "screen -S #{name} -p 0 -X kill"
sleep 1
system "screen -S #{name} -p 0 -X exec /Users/andrewroth/Development/rockbox_workout/auto_update.sh"
sleep 1


