class RenameExerciseLogEntriesWorkoutLogEntryId < ActiveRecord::Migration
  def self.up
    rename_column :exercise_log_entries, :workout_log_entry_id, :workout_date_id
  end

  def self.down
    rename_column :exercise_log_entries, :workout_date_id, :workout_log_entry_id
  end
end
