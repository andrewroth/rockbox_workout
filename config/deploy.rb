set :application, "workout"
set :repository,  "git://github.com/andrewroth/rockbox_workout.git"
set :scm, "git"
set :use_sudo, true
set :user, "deploy"
set :group, "www-data"
set :password, File.read("password.txt").chomp
#set :port, 40022
default_run_options[:pty] = true

set :deploy_to, "/var/www/workout.andrewroth.ca"

#role :app, "173.203.111.74"
#role :web, "173.203.111.74"
#role :db, "173.203.111.74", :primary => true
role :app, "173.203.106.17"
role :web, "173.203.106.17"
role :db, "173.203.106.17", :primary => true

set :branch, "master"
set :deploy_via, :remote_cache
set :git_enable_submodules, 1

desc "Restart the web server"
deploy.task :restart, :roles => :app do
    run "touch #{deploy_to}/current/tmp/restart.txt"
end 

after 'deploy:update_code', 'local_symlinks'
desc "Add linked files after deploy and set permissions"
task :local_symlinks, :roles => :app do
  run "ln -s #{shared_path}/config/database.yml #{release_path}/config/database.yml"
  run "ln -s #{shared_path}/config/initializers/auth.rb #{release_path}/config/initializers/auth.rb"
  run "ln -s #{shared_path}/tmp/sessions #{release_path}/tmp/sessions"
  run "mkdir #{release_path}/transfer"
  #run "cd #{release_path} && /opt/ruby/bin/bundle install"
  sudo "chgrp -R #{fetch(:group)} #{release_path}"
  sudo "chmod -R g+w #{release_path}"
end

desc "Automatically run cleanup"
after 'deploy', 'deploy:cleanup'

task :long_deploy do
  transaction do
    deploy.update_code
    # deploy.disable_web
    deploy.symlink
    deploy.migrate
  end

  deploy.restart
  deploy.cleanup
  # deploy.enable_web
end


namespace :sync do
  task :default do
    up
    down
  end

  task :download do
    run("cd #{deploy_to}/current && /usr/bin/env rake csv:dump RAILS_ENV=production")
    for file_name in %w(workouts workout_dates workout_exercises exercises exercise_sets functions exercise_logs set_logs)
      get("#{deploy_to}/current/transfer/#{file_name}.csv", "transfer/#{file_name}.csv")
    end
  end

  task :vmware do
    download
    vm.flash
  end

  desc "downloads al the csvs from the server and loads them to the sansa"
  task :down do
    download
    sansa.flash
  end

  desc "uploads the local csvs in transfer and loads them into the server db"
  task :up do
    sansa.pull
    run "mkdir -p #{deploy_to}/current/transfer"
    for file_name in %w(workout_dates exercise_logs set_logs)
      upload("transfer/#{file_name}.csv", "#{deploy_to}/current/transfer/#{file_name}.csv")
    end
    run("cd #{deploy_to}/current && /usr/bin/env rake csv:read RAILS_ENV=production")
  end
end

namespace :sansa do
  desc "copies transfer/*.csv to sansa"
  task :flash do
    ensure_plugged_in
    system "sh #{File.dirname(__FILE__)}/../copy_csvs_to_sansa.sh"
  end
  desc "pulls the csv from sansa to local transfer dir"
  task :pull do
    ensure_plugged_in
    system "sh #{File.dirname(__FILE__)}/../copy_csvs_from_sansa.sh"
  end
  task :ensure_plugged_in do
    until File.directory?("/Volumes/Sansa\ e260")
      puts "Plug in sansa and hit enter."
      STDIN.gets
    end
  end
end

namespace :vm do
  task :default do
    flash
  end

  task :flash do
    system "sh #{File.dirname(__FILE__)}/../copy_csvs_to_vmware.sh"
  end
end

namespace :db do
  task :pull do
    STDOUT.print "This will destroy the local db and download it from the server, are you sure? (y/n) "
    confirm = STDIN.gets
    unless confirm.to_s.downcase == 'y'
      run("cd #{deploy_to}/current && /usr/bin/env rake db:dump RAILS_ENV=production")
      get("/home/deploy/workout.sql", "workout.sql")
      system("rake db:load_dump")
    end
  end
end

task :demo do
  set :user, "deploy"
  set :deploy_to, "/var/www/workoutdemo.andrewroth.ca"
end
