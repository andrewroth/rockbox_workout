class CreateExerciseLogEntries < ActiveRecord::Migration
  def self.up
    create_table :exercise_log_entries do |t|
      t.integer :workout_log_entry_id
      t.integer :workout_exercise_id
      t.integer :n
      t.integer :position

      t.timestamps
    end
  end

  def self.down
    drop_table :exercise_log_entries
  end
end
