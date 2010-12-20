class RenameExerciseLogEntryWorkoutExerciseToExercise < ActiveRecord::Migration
  def self.up
    rename_column :exercise_log_entries, :workout_exercise_id, :exercise_id
  end

  def self.down
    remove_column :exercise_log_entries, :workout_exercise_id
  end
end
