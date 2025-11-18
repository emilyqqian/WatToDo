import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  TextField,
} from '@mui/material'

import { useState } from 'react'
import { updateUser } from '../APIManager'
import { useGlobal } from '../SessionManager'
import { getStringHashCode } from '../APIManager'

function SetPassword({ open, onClose}) {
    const [password, setPassword] = useState('')
    const [confirmedPassword, setConfirmedPassword] = useState('')
    const { state } = useGlobal();

  const handleClose = () => {
    onClose()
    setPassword('')
    setConfirmedPassword('')
  }

  const handleSave = () => {
    if (password === confirmedPassword){
        updateUser({
            username: state.user.username,
            password: getStringHashCode(password),
            xp_points: state.user.xp
        }, state.user.userId).then(data => {
            if (data) handleClose()
        })
        return;
    }

    alert("Password Doesn't Match!")
  }

  return (
    <Dialog open={open} onClose={handleClose} fullWidth maxWidth="sm">
      <DialogTitle>Change Password:</DialogTitle>
      <DialogContent sx={{mt: '3%'}}>
        <TextField
            label="Password"
            type="password"
            autoComplete="current-password"
            value={password}
            onChange={(event) => setPassword(event.target.value)}
            fullWidth
            sx={{mb: '5%'}}
        />
        
        <TextField
            label="Confirm Password"
            type="password"
            autoComplete="current-password"
            value={confirmedPassword}
            onChange={(event) => setConfirmedPassword(event.target.value)}
            fullWidth
            sx={{mb: '5%'}}
        />
      </DialogContent>
      <DialogActions sx={{ px: 3, pb: 2 }}>
        <Button onClick={handleClose}>Cancel</Button>
        <Button variant="contained" onClick={handleSave}>
          Save Password
        </Button>
      </DialogActions>
    </Dialog>
  )
}

export default SetPassword;