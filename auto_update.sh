#!/usr/bin/ruby

SYNC_DELAY = 300
CHECK_DELAY = 5

while (true)
  puts "Checking..."
  if File.exists?("/Volumes/Sansa e260")
    puts "Found!"
    system("cd /Users/andrewroth/Development/rockbox_workout && cap sync")
    sleep SYNC_DELAY
  end
  sleep CHECK_DELAY
end
