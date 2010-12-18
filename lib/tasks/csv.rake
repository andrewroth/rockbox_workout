namespace :csv do
  task :dump => :environment do
    Workout.write_csv
    Exercise.write_csv
    WorkoutExercise.write_csv
    WorkoutDate.write_csv
    ExerciseSet.write_csv
    Function.write_csv
    ExerciseLogEntry.write_csv
    SetLogEntry.write_csv
  end

  task :read => :environment do
    WorkoutDate.read_csv
    ExerciseLogEntry.read_csv
    SetLogEntry.read_csv
  end
end
