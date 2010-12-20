desc "cleans up data"
task :sanitize do
  ExerciseLogEntry.all.find_all{ |ele| ele.exercise.nil? }.each do |ele| ele.destroy end
end
