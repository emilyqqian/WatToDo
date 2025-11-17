// GlobalState.jsx
import { createContext, useContext, useState } from "react";

const GlobalContext = createContext();

export function GlobalProvider({ children }) {
  const [state, setState] = useState({
    user: null,
    loggedIn: false
  });

  const updateState = (partial) =>
    setState((prev) => ({ ...prev, ...partial }));

  return (
    <GlobalContext.Provider value={{ state, updateState }}>
      {children}
    </GlobalContext.Provider>
  );
}

export function useGlobal() {
  return useContext(GlobalContext);
}
