class CreateWorkoutExercises < ActiveRecord::Migration
  def self.up
    create_table :workout_exercises do |t|
      t.integer :workout_id
      t.integer :exercise_id
      t.integer :position

      t.timestamps
    end
  end

  def self.down
    drop_table :workout_exercises
  end
end
