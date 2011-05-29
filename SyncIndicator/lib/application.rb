require 'rubygems'
require 'hotcocoa'

# Replace the following code with your own hotcocoa code

class Application

  include HotCocoa
  
  SYNC_DELAY = 300
  CHECK_DELAY = 5

  def start
    puts "HERE"

    application :name => "Syncindicator" do |app|
      app.delegate = self
      window :frame => [100, 100, 500, 500], :title => "Syncindicator" do |win|
        win << @l = label(:text => "This is a very long message so that the text field gets a default layout width.", :layout => {:start => false})
        win.will_close { exit }
        @win = win
      end
      t = Thread.new do
        check
      end
    end
  end
  
  def output_message(m)
    puts m
    @l.text = m
    @win.display
  end

  def check
    while (true)
      @win.miniaturize(0)
      output_message "Checking..."
      if File.exists?("/Volumes/Sansa e260")
        @win.deminiaturize(0)
        output_message "Found!  Syncing..."
        system("cd /Users/andrewroth/Development/rockbox_workout && cap sync")
        #p = IO.popen("cd /Users/andrewroth/Development/rockbox_workout && cap sync")
        output_message "Done.  Waiting #{SYNC_DELAY} seconds before trying again."
        sleep 3
        @win.miniaturize(0)
        sleep SYNC_DELAY
      end
      sleep CHECK_DELAY
    end
  end

  # file/open
  def on_open(menu)
  end
  
  # file/new 
  def on_new(menu)
  end
  
  # help menu item
  def on_help(menu)
  end
  
  # This is commented out, so the minimize menu item is disabled
  #def on_minimize(menu)
  #end
  
  # window/zoom
  def on_zoom(menu)
  end
  
  # window/bring_all_to_front
  def on_bring_all_to_front(menu)
  end
end

app = Application.new
app.start
app.check
