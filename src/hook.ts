import {
  Dispatch,
  MutableRefObject,
  useCallback,
  useRef,
  useState,
} from "react";

export function useForceUpdate() {
  const [_, setState] = useState(true);
  return useCallback(() => {
    setState((s) => !s);
  }, []);
}

export function useRefState<T>(init: T): [MutableRefObject<T>, Dispatch<T>] {
  const ref = useRef<T>(init);
  const forceUpdate = useForceUpdate();
  const dispatch = (next: T) => {
    ref.current = next;
    forceUpdate();
  };
  return [ref, dispatch];
}
