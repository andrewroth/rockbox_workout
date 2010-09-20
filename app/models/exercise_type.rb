class ExerciseType < ActiveRecord::Base
  def variables
    [ v1, v2, v3, v4, v5, v6, v7 ].delete_if{ |v| v.nil? || v.empty? }
  end
end
