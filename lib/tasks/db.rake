namespace :db do
  task :dump => :environment do
    password = ActiveRecord::Base.configurations[RAILS_ENV]['password']
    db = ActiveRecord::Base.configurations[RAILS_ENV]['database']
    system "mysqldump --user root --password=#{password} > /home/deploy/#{db}.sql"
  end

  task :load_dump => :environment do
    password = ActiveRecord::Base.configurations[RAILS_ENV]['password']
    db = ActiveRecord::Base.configurations[RAILS_ENV]['database']
    system "echo 'drop database if exists #{db}; create database #{db};' | mysql --user root --password=#{password}"
    system "cat #{db}.sql | mysql --user root --password=#{password}"
  end
end
