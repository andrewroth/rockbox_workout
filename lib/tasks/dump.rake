namespace :csv do
  task :dump => :environment do
    Workout.write_csv
    Exercise.write_csv
    WorkoutExercise.write_csv
    WorkoutDate.write_csv
    ExerciseSet.write_csv
  end
end
