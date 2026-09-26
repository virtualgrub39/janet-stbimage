(import _stbimage :as core)

(defn load [path]
    (let [st os/stat path]
        (when (and st (= (st :mode) :file))
            (error (string "File not found: " path))))
    (core/load path))
