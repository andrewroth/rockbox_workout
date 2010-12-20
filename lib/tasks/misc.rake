desc "cleans up data"
task :sanitize => :environment do
  ExerciseLogEntry.all.find_all{ |ele| ele.exercise.nil? }.each do |ele| ele.destroy end
  SetLogEntry.all.find_all{ |sle| sle.exercise_log_entry.nil? }.each do |sle| sle.destroy end
end
